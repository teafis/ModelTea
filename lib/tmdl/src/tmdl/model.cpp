// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/model.hpp>

using namespace tmdl;

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
    auto conn_it = connections.begin();
    while (conn_it != connections.end())
    {
        if ((*conn_it)->contains_id(id))
        {
            if (const auto fid = (*conn_it)->get_from_id(); fid != id)
            {
                auto blk = get_block(fid);
                blk->set_output_port((*conn_it)->get_from_port(), nullptr);
            }

            if (const auto tid = (*conn_it)->get_to_id(); tid != id)
            {
                auto blk = get_block(tid);
                blk->set_input_port((*conn_it)->get_to_port(), nullptr);
            }

            conn_it = connections.erase(conn_it);
        }
        else
        {
            ++conn_it;
        }
    }
}

void Model::add_connection(const Connection connection)
{
    BlockInterface* from_block = get_block(connection.get_from_id());
    BlockInterface* to_block = get_block(connection.get_to_id());

    if (connection.get_from_port() < from_block->get_num_outputs() &&
        connection.get_to_port() < to_block->get_num_inputs())
    {
        connections.push_back(std::make_unique<Connection>(connection));
        auto& c = connections.back();

        from_block->set_output_port(
            connection.get_from_port(),
            &c->get_value());

        to_block->set_input_port(
            c->get_to_port(),
            &c->get_value());
    }
    else
    {
        throw ModelException("to/from block and port number mismatch");
    }
}

void Model::remove_connection(const size_t to_block, const size_t to_port)
{
    auto it = std::find_if(
        connections.begin(),
        connections.end(),
        [to_block, to_port](const Connection& c)
    {
        return c.get_to_port() == to_port && c.get_to_id() == to_block;
    });

    if (it == connections.end())
    {
        throw ModelException("connection not found to be able to remove");
    }

    connections.erase(it);
}

size_t Model::get_num_inputs() const
{
    return input_ids.size();
}

size_t Model::get_num_outputs() const
{
    return output_ids.size();
}

std::shared_ptr<BlockExecutionInterface> Model::get_execution_interface() const
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
            const auto conn_it = std::find_if(
                connections.begin(),
                connections.end(),
                [id, port](const Connection& c)
            {
                return c.get_to_id() == id && c.get_to_port() == port;
            });

            if (conn_it == connections.end())
            {
                throw ModelException("cannot compute execution order for incomplete input ports");
            }
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

            // Search for each port to see if it is complete
            for (size_t port = 0; block->get_num_inputs(); ++port)
            {
                // Find the corresponding connection
                const auto conn_it = std::find_if(
                    connections.begin(),
                    connections.end(),
                    [id, port](const Connection& c)
                {
                    return c.get_to_port() == port && c.get_to_id() == id;
                });

                if (conn_it == connections.end())
                {
                    throw ModelException("cannot compute execution order for incomplete input ports");
                }

                // Skip if the input port is a delayed input, but after the connection
                // to ensure that we check that all blocks are connected
                if (block->is_delayed_input(port))
                {
                    continue;
                }

                // Check if the from block is already in the execution order
                const auto from_it = std::find(
                    order_values.begin(),
                    order_values.end(),
                    conn_it->get_from_id());

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

    // Construct the interface order value
    std::vector<std::shared_ptr<BlockExecutionInterface>> interface_order;
    for (const auto& b_id : order_values)
    {
        std::shared_ptr<BlockExecutionInterface> block = get_block(b_id)->get_execution_interface();
        interface_order.push_back(block);
    }

    // Create the executor
    return std::make_shared<Model::ModelExecutor>(this, interface_order);
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

/* ==================== MODEL EXECUTOR ==================== */

Model::ModelExecutor::ModelExecutor(
    const Model* parent,
    const std::vector<std::shared_ptr<BlockExecutionInterface>>& blocks) :
    BlockExecutionInterface(parent),
    blocks(blocks)
{
    // Add input ports and output ports
    for (const auto& b : blocks)
    {
        const auto input_test = std::dynamic_pointer_cast<InputPort::Executor>(b);
        const auto output_test = std::dynamic_pointer_cast<OutputPort::Executor>(b);

        if (input_test != nullptr)
        {
            input_blocks.push_back(input_test);
        }

        if (output_test != nullptr)
        {
            output_blocks.push_back(output_test);
        }
    }

    // Check numbers for each
    if (input_blocks.size() != get_num_inputs())
    {
        throw ModelException("number of input ports does not match input block count");
    }

    if (output_blocks.size() != get_num_outputs())
    {
        throw ModelException("number of output ports does not match input block count");
    }
}

void Model::ModelExecutor::set_input_value(
    const size_t port,
    const void* value)
{
    if (port < input_blocks.size())
    {
        input_blocks[port]->set_value(value);
    }
}

const void* Model::ModelExecutor::get_output_value(const size_t port) const
{
    if (port < output_blocks.size())
    {
        return output_blocks[port]->get_valu();
    }
    else
    {
        throw ModelException("requested port exceeds model size");
    }
}

void Model::ModelExecutor::step()
{
    // Step each block (already in execution order)
    for (const auto& b : blocks)
    {
        b->step();
    }
}

void Model::ModelExecutor::reset()
{
    for (const auto& b : blocks)
    {
        b->reset();
    }
}
