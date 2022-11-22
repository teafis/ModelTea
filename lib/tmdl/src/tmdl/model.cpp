// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/model.hpp>

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
            from_block->get_output_port(connection.get_from_port()));
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
        blk->set_input_port(c.get_to_port(), PortValue());
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
                from_blk->get_output_port(c.get_from_port()));
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
    const PortValue value)
{
    if (port < get_num_inputs())
    {
        auto* blk = dynamic_cast<InputPort*>(get_block(input_ids[port]));
        if (blk == nullptr)
        {
            throw ModelException("invalid block found for input port");
        }

        blk->set_input_value(value);
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
    const ConnectionManager&,
    const VariableManager&) const
{
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
                if (block->is_delayed_input(port))
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

    for (const auto& c : connections.get_connections())
    {
        const VariableIdentifier vid {
            .block_id = c.get_from_id(),
            .output_port_num = c.get_from_port()
        };

        const PortValue pv = get_block(vid.block_id)->get_output_port(vid.output_port_num);

        std::shared_ptr<ValueBox> value;

        switch (pv.dtype)
        {
        case DataType::BOOLEAN:
            value = std::make_shared<ValueBoxType<bool>>(false);
            break;
        case DataType::SINGLE:
            value = std::make_shared<ValueBoxType<float>>(false);
            break;
        case DataType::DOUBLE:
            value = std::make_shared<ValueBoxType<double>>(false);
            break;
        case DataType::INT32:
            value = std::make_shared<ValueBoxType<int32_t>>(false);
            break;
        case DataType::UINT32:
            value = std::make_shared<ValueBoxType<uint32_t>>(false);
            break;
        default:
            throw ModelException("unable to construct value for type");
        }

        variables.add_variable(vid, value);
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
    return std::make_shared<ModelExecutor>(interface_order);
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
