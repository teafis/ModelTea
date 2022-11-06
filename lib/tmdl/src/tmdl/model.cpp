// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/model.hpp>

using namespace tmdl;

void Model::add_block(std::unique_ptr<BlockInterface> block)
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

    blocks.insert({new_id, std::move(block)});

    execution_order.reset();
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
        if (conn_it->contains_id(id))
        {
            conn_it = connections.erase(conn_it);
        }
        else
        {
            ++conn_it;
        }
    }

    // Reset execution order
    execution_order.reset();
}

void Model::add_connection(const Connection connection)
{
    const BlockInterface* from_block = get_block(connection.get_from_id());
    const BlockInterface* to_block = get_block(connection.get_to_id());

    if (connection.get_from_port() < from_block->get_num_outputs() &&
        connection.get_to_port() < to_block->get_num_inputs())
    {
        connections.push_back(connection);
        execution_order.reset();
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
    execution_order.reset();
}

size_t Model::get_num_inputs() const
{
    return input_ids.size();
}

size_t Model::get_num_outputs() const
{
    return output_ids.size();
}

void Model::set_input_value(
    const size_t port,
    std::unique_ptr<Value> value)
{
    if (port < input_ids.size())
    {
        const size_t id = input_ids[port];
        const auto it = blocks.find(id);

        if (it == blocks.end())
        {
            throw ModelException("unable to find input port parameter");
        }

        it->second->set_input_value(0, std::move(value));
    }
}

std::unique_ptr<Value> Model::get_output_value(const size_t port) const
{
    if (port < output_ids.size())
    {
        const size_t id = output_ids[port];
        const auto it = blocks.find(id);

        if (it == blocks.end())
        {
            throw ModelException("output port ID does not exist");
        }

        return it->second->get_output_value(0);
    }
    else
    {
        throw ModelException("requested port exceeds model size");
    }
}

void Model::step()
{
    if (!execution_order.has_value())
    {
        compile_execution_order();
    }

    for (const auto id : execution_order.value())
    {
        const auto& block = get_block(id);

        for (size_t port = 0; port < block->get_num_inputs(); ++port)
        {
            const auto c_it = std::find_if(
                connections.begin(),
                connections.end(),
                [id, port](const Connection& c)
            {
                return c.get_to_id() == id && c.get_to_port() == port;
            });

            if (c_it == connections.end())
            {
                throw ModelException("unable to set input port parameters for block");
            }

            block->set_input_value(
                port,
                get_block(c_it->get_from_id())->get_output_value(c_it->get_from_port()));
        }

        block->step();
    }
}

void Model::reset()
{
    for (const auto& b : blocks)
    {
        b.second->reset();
    }
}

void Model::compile_execution_order()
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
        if (std::find(input_ids.begin(), input_ids.end(), it.first) == input_ids.end())
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
            const size_t id = remaining_id_values[i];
            const BlockInterface* block = get_block(id);

            for (size_t port = 0; block->get_num_inputs(); ++port)
            {
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

    execution_order = order_values;
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
