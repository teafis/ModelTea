// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CONNECTION_MANAGER_HPP
#define TF_MODEL_CONNECTION_MANAGER_HPP

#include "connection.hpp"

#include <vector>


namespace tmdl
{

class ConnectionManager
{
public:
    void add_connection(const Connection& c);

    void remove_block(const size_t block_id);

    void remove_connection(const size_t to_block, const size_t to_port);

    const Connection& get_connection_to(const size_t to_block, const size_t to_port) const;

    bool has_connection_to(const size_t to_block, const size_t to_port) const;

    const std::vector<Connection>& get_connections() const;

protected:
    std::vector<Connection> connections;
};

}

#endif // TF_MODEL_CONNECTION_MANAGER_HPP
