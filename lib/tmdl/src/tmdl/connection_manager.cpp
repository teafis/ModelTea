// SPDX-License-Identifier: GPL-3.0-only

#include "connection_manager.hpp"

#include "model_exception.hpp"

#include <algorithm>

std::vector<tmdl::Connection>::const_iterator get_iter_for_block(
    const std::vector<tmdl::Connection>& connections,
    const size_t to_block,
    const size_t to_port)
{
    return std::find_if(connections.begin(), connections.end(), [to_block,to_port](const tmdl::Connection& c)
    {
        return c.get_to_id() == to_block && c.get_to_port() == to_port;
    });
}

void tmdl::ConnectionManager::add_connection(const Connection& c)
{
    const auto it = get_iter_for_block(connections, c.get_to_id(), c.get_to_port());

    if (it != connections.end())
    {
        throw ModelException("duplicate connection provided");
    }
    else
    {
        connections.push_back(c);
    }
}

void tmdl::ConnectionManager::remove_block(const size_t block_id)
{
    size_t i = 0;
    while (i < connections.size())
    {
        const Connection& c = connections[i];

        if (c.get_to_id() == block_id || c.get_from_id() == block_id)
        {
            connections.erase(connections.begin() + i);
        }
        else
        {
            i += 1;
        }
    }
}

void tmdl::ConnectionManager::remove_connection(const size_t to_block, const size_t to_port)
{
    const auto it = get_iter_for_block(connections, to_block, to_port);

    if (it != connections.end())
    {
        connections.erase(it);
    }
    else
    {
        throw ModelException("no connection found for provided block port");
    }
}

const tmdl::Connection& tmdl::ConnectionManager::get_connection_to(const size_t to_block, const size_t to_port) const
{
    const auto it = get_iter_for_block(connections, to_block, to_port);

    if (it != connections.end())
    {
        return *it;
    }
    else
    {
        throw ModelException("no connection found for provided block port");
    }
}

bool tmdl::ConnectionManager::has_connection_to(const size_t to_block, const size_t to_port) const
{
    return get_iter_for_block(connections, to_block, to_port) != connections.end();
}

const std::vector<tmdl::Connection>& tmdl::ConnectionManager::get_connections() const
{
    return connections;
}

void tmdl::to_json(nlohmann::json& j, const ConnectionManager& cm)
{
    j = cm.get_connections();
}

void tmdl::from_json(const nlohmann::json& j, ConnectionManager& cm)
{
    for (const auto& i : j)
    {
        tmdl::Connection c(0, 0, 0, 0);
        from_json(i, c);
        cm.add_connection(c);
    }
}
