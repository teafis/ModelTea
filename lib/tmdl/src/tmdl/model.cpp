// SPDX-License-Identifier: GPL-3.0-only

#include "model.hpp"

#include "blocks/io_ports.hpp"
#include "model_exception.hpp"

using namespace tmdl;

/* ==================== MODEL EXECUTOR ==================== */

class ModelExecutor : public BlockExecutionInterface
{
public:
    ModelExecutor(const std::vector<std::shared_ptr<BlockExecutionInterface>>& blocks) :
        blocks(blocks)
    {
        // Empty Constructor?
    }

    void init() override
    {
        for (const auto& b : blocks)
        {
            b->init();
        }
    }

    void step(const SimState& state) override
    {
        for (const auto& b : blocks)
        {
            b->step(state);
        }
    }

    void reset() override
    {
        for (const auto& b : blocks)
        {
            b->reset();
        }
    }

    void close() override
    {
        for (const auto& b : blocks)
        {
            b->close();
        }
    }

protected:
    std::vector<std::shared_ptr<BlockExecutionInterface>> blocks;
};


/* ==================== MODEL ==================== */

void Model::add_block(const std::shared_ptr<BlockInterface> block)
{
    const size_t new_id = get_next_id();

    if (const auto* ptr = dynamic_cast<InputPort*>(block.get()); ptr != nullptr)
    {
        input_ids.push_back(new_id);
    }
    else if (const auto* ptr = dynamic_cast<OutputPort*>(block.get()); ptr != nullptr)
    {
        output_ids.push_back(new_id);
    }

    block->set_id(new_id);
    blocks.insert({new_id, std::move(block)});
}

void Model::remove_block(const size_t id)
{
    // Search for the block in the input values
    auto map_it = blocks.find(id);
    if (map_it == blocks.end())
    {
        throw ModelException("id not found in the block map");
    }

    blocks.erase(map_it);

    // Remove references in the port vectors
    for (auto* vec : { &input_ids, &output_ids })
    {
        auto it = std::find(
            vec->begin(),
            vec->end(),
            id);
        if (it != vec->end())
        {
            vec->erase(it);
        }
    }

    // Remove references to the block ID
    connections.remove_block(id);
}

void Model::add_connection(const Connection connection)
{
    const BlockInterface* from_block = get_block(connection.get_from_id());
    BlockInterface* to_block = get_block(connection.get_to_id());

    if (connection.get_from_port() < from_block->get_num_outputs() &&
        connection.get_to_port() < to_block->get_num_inputs())
    {
        connections.add_connection(connection);
        to_block->set_input_port(
            connection.get_to_port(),
            from_block->get_output_port(connection.get_from_port()).dtype);

        to_block->update_block();
    }
    else
    {
        throw ModelException("to/from block and port number mismatch");
    }
}

void Model::remove_connection(const size_t to_block, const size_t to_port)
{
    const auto& c = connections.get_connection_to(to_block, to_port);

    auto* blk = get_block(c.get_to_id());
    if (c.get_to_port() < blk->get_num_inputs())
    {
        blk->set_input_port(c.get_to_port(), DataType::UNKNOWN);
    }

    connections.remove_connection(to_block, to_port);
}

size_t Model::get_num_inputs() const
{
    return input_ids.size();
}

size_t Model::get_num_outputs() const
{
    return output_ids.size();
}

bool Model::update_block()
{
    bool any_updated = true;
    bool model_updated = false;
    size_t update_count = 0;
    const size_t UPDATE_LIMIT = blocks.size() * 2;

    while (any_updated)
    {
        // Set all to not updated
        any_updated = false;

        // Update any port types
        for (const auto& c : connections.get_connections())
        {
            const auto* from_blk = get_block(c.get_from_id());
            auto* to_blk = get_block(c.get_to_id());

            to_blk->set_input_port(
                c.get_to_port(),
                from_blk->get_output_port(c.get_from_port()).dtype);
        }

        // Check each port for updates
        for (auto& blk : blocks)
        {
            any_updated |= blk.second->update_block();
        }

        // Mark as updated, and break if the count has been exceeded
        if (any_updated)
        {
            model_updated = true;
        }

        if (update_count > UPDATE_LIMIT)
        {
            throw ModelException("update limit exceeded for model block");
        }

        update_count += 1;
    }

    return model_updated;
}

std::unique_ptr<const BlockError> Model::has_error() const
{
    for (const auto& b : blocks)
    {
        auto blk_error = b.second->has_error();
        if (blk_error != nullptr)
        {
            return blk_error;
        }
    }

    return nullptr;
}

void Model::set_input_port(
    const size_t port,
    const DataType type)
{
    if (port < get_num_inputs())
    {
        auto* blk = dynamic_cast<InputPort*>(get_block(input_ids[port]));
        if (blk == nullptr)
        {
            throw ModelException("invalid block found for input port");
        }

        blk->set_input_value(type);
    }
    else
    {
        throw ModelException("model input port count exceeded");
    }
}

PortValue Model::get_output_port(const size_t port) const
{

    if (port < get_num_outputs())
    {
        auto* blk = dynamic_cast<OutputPort*>(get_block(output_ids[port]));
        if (blk == nullptr)
        {
            throw ModelException("invalid block found for input port");
        }

        return blk->get_output_value();
    }
    else
    {
        throw ModelException("model input port count exceeded");
    }
}

std::shared_ptr<BlockExecutionInterface> Model::get_execution_interface(
    const ConnectionManager& outer_connections,
    const VariableManager& outer_variables) const
{
    // Skip if an error is present
    if (const auto err = has_error(); err != nullptr)
    {
        throw ModelException(err->message);
    }

    // Add blocks for input and output ports


    // Check that all inputs are connected
    for (const auto& b : blocks)
    {
        const auto id = b.first;

        if (std::find(input_ids.begin(), input_ids.end(), id) != input_ids.end())
        {
            continue;
        }

        const auto& block = b.second;

        for (size_t port = 0; port < block->get_num_inputs(); ++port)
        {
            connections.has_connection_to(id, port);
        }
    }

    // Add the input blocks as the first to be executed
    std::vector<size_t> order_values;
    for (const size_t i : input_ids)
    {
        order_values.push_back(i);
    }

    // Create a list of remaining ID values
    std::vector<size_t> remaining_id_values;
    for (const auto& it : blocks)
    {
        const bool is_input = std::find(input_ids.begin(), input_ids.end(), it.first) != input_ids.end();
        const bool is_output = std::find(output_ids.begin(), output_ids.end(), it.first) != output_ids.end();

        if (!is_input && !is_output)
        {
            remaining_id_values.push_back(it.first);
        }
    }

    // Add blocks to the list that have all input ports accounted for
    while (remaining_id_values.size() > 0)
    {
        std::optional<size_t> index;

        for (size_t i = 0; i < remaining_id_values.size(); ++i)
        {
            // Extract identifiers
            const size_t id = remaining_id_values[i];
            const BlockInterface* block = get_block(id);

            // Add the index if there are no input ports
            if (block->get_num_inputs() == 0)
            {
                index = i;
                break;
            }

            // Search for each port to see if it is complete
            for (size_t port = 0; port < block->get_num_inputs(); ++port)
            {
                // Ensure that all parameters are accounted for
                if (!connections.has_connection_to(id, port))
                {
                    throw ModelException("cannot compute execution order for incomplete input ports");
                }

                // Skip if the input port is a delayed input, but after need to check
                // connections to ensure that all blocks are connected
                const auto from_conn = connections.get_connection_to(id, port);
                const auto from_port = get_block(from_conn.get_from_id())->get_output_port(from_conn.get_from_port());

                if (from_port.is_delayed_output)
                {
                    continue;
                }

                // Grab the port
                const auto& conn = connections.get_connection_to(id, port);

                // Check if the from block is already in the execution order
                const auto from_it = std::find(
                    order_values.begin(),
                    order_values.end(),
                    conn.get_from_id());

                if (from_it == order_values.end())
                {
                    continue;
                }
                else
                {
                    index = i;
                    break;
                }
            }
        }

        if (index.has_value())
        {
            auto it = remaining_id_values.begin() + index.value();
            order_values.push_back(*it);
            remaining_id_values.erase(it);
        }
        else
        {
            throw ModelException("unable to solve - no block found to run execution values...");
        }
    }

    // Add remaining output ID values
    for (const size_t i : output_ids)
    {
        order_values.push_back(i);
    }

    // Construct the variable list values
    VariableManager variables;

    // Add output port types
    for (size_t i = 0; i < output_ids.size(); ++i)
    {
        const auto outer_id = VariableIdentifier {
            .block_id = get_id(),
            .output_port_num = i
        };

        const auto ptr_val = outer_variables.get_ptr(outer_id);

        const auto& c = connections.get_connection_to(output_ids[i], 0);
        const auto inner_id = VariableIdentifier
        {
            .block_id = c.get_from_id(),
            .output_port_num = c.get_from_port()
        };

        variables.add_variable(inner_id, ptr_val);
    }

    // Add input port types
    for (size_t i = 0; i < input_ids.size(); ++i)
    {
        const auto& outer_connection = outer_connections.get_connection_to(get_id(), i);
        const auto ptr_val = outer_variables.get_ptr(outer_connection);

        const auto inner_id = VariableIdentifier
        {
            .block_id = input_ids[i],
            .output_port_num = 0
        };

        variables.add_variable(inner_id, ptr_val);
    }

    // Add interior block types
    for (const auto& bv : blocks)
    {
        // Grab block, but skip if an input or output port, as it would have been updated above
        const auto blk = bv.second;

        for (size_t i = 0; i < blk->get_num_outputs(); ++i)
        {
            const VariableIdentifier vid {
                .block_id = blk->get_id(),
                .output_port_num = i
            };

            // Skip if variable already added (due to input/output)
            if (!variables.has_variable(vid))
            {
                const auto pv = get_block(vid.block_id)->get_output_port(vid.output_port_num);
                variables.add_variable(vid, make_shared_default_value(pv.dtype));
            }
        }
    }

    // Construct the interface order value
    std::vector<std::shared_ptr<BlockExecutionInterface>> interface_order;
    for (const auto& b_id : order_values)
    {
        std::shared_ptr<BlockExecutionInterface> block = get_block(b_id)->get_execution_interface(
            connections,
            variables);
        interface_order.push_back(block);
    }

    // Create the executor
    auto model_exec = std::make_shared<ModelExecutor>(interface_order);

    // Reset the model executor
    model_exec->reset();

    // Return result
    return model_exec;
}

std::vector<std::unique_ptr<const BlockError>> Model::get_all_errors() const
{
    std::vector<std::unique_ptr<const BlockError>> error_vals;

    for (const auto& kv : blocks)
    {
        const auto blk = kv.second;
        auto err = blk->has_error();
        if (err != nullptr)
        {
            error_vals.push_back(std::move(err));
        }
    }

    return error_vals;
}

size_t Model::get_next_id() const
{
    size_t current_id = 0;
    while (blocks.find(current_id) != blocks.end())
    {
        current_id += 1;
    }

    return current_id;
}

BlockInterface* Model::get_block(const size_t id) const
{
    const auto it = blocks.find(id);
    if (it == blocks.end())
    {
        throw ModelException("unable to find block with given id");
    }
    return it->second.get();
}
