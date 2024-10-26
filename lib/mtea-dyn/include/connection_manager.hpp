// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNCONNECTION_MANAGER_HPP
#define MTEA_DYNCONNECTION_MANAGER_HPP

#include "connection.hpp"

#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

namespace mtea {

class ConnectionManager {
public:
    void add_connection(const std::shared_ptr<Connection> c);

    void remove_block(const size_t block_id);

    void remove_connection(const size_t to_block, const size_t to_port);

    std::shared_ptr<Connection> get_connection_to(const size_t to_block, const size_t to_port) const;

    bool has_connection_to(const size_t to_block, const size_t to_port) const;

    std::vector<std::shared_ptr<const Connection>> get_connections() const;

private:
    std::vector<std::shared_ptr<Connection>> connections;
};

void to_json(nlohmann::json& j, const ConnectionManager& cm);

void from_json(const nlohmann::json& j, ConnectionManager& cm);

}

#endif // MTEA_DYNCONNECTION_MANAGER_HPP
