// SPDX-License-Identifier: GPL-3.0-only

#include "connection_manager.hpp"

#include "model_exception.hpp"

#include <algorithm>

std::vector<std::shared_ptr<mtea::Connection>>::const_iterator
get_iter_for_block(const std::vector<std::shared_ptr<mtea::Connection>>& connections, const size_t to_block, const size_t to_port) {
    return std::ranges::find_if(connections,
                                [to_block, to_port](const auto& c) { return c->get_to_id() == to_block && c->get_to_port() == to_port; });
}

void mtea::ConnectionManager::add_connection(const std::shared_ptr<Connection> c) {
    if (c == nullptr) {
        throw ModelException("cannot add a null connection");
    }

    const auto it = get_iter_for_block(connections, c->get_to_id(), c->get_to_port());

    if (it != connections.end()) {
        throw ModelException("duplicate connection provided");
    } else {
        connections.push_back(c);
    }
}

void mtea::ConnectionManager::remove_block(const size_t block_id) {
    size_t i = 0;
    while (i < connections.size()) {
        const auto& c = connections[i];

        if (c->get_to_id() == block_id || c->get_from_id() == block_id) {
            connections.erase(connections.begin() + i);
        } else {
            i += 1;
        }
    }
}

void mtea::ConnectionManager::remove_connection(const size_t to_block, const size_t to_port) {
    const auto it = get_iter_for_block(connections, to_block, to_port);

    if (it != connections.end()) {
        connections.erase(it);
    } else {
        throw ModelException("no connection found for provided block port");
    }
}

std::shared_ptr<mtea::Connection> mtea::ConnectionManager::get_connection_to(const size_t to_block, const size_t to_port) const {
    const auto it = get_iter_for_block(connections, to_block, to_port);

    if (it != connections.end()) {
        return *it;
    } else {
        throw ModelException("no connection found for provided block port");
    }
}

bool mtea::ConnectionManager::has_connection_to(const size_t to_block, const size_t to_port) const {
    return get_iter_for_block(connections, to_block, to_port) != connections.end();
}

std::vector<std::shared_ptr<const mtea::Connection>> mtea::ConnectionManager::get_connections() const {
    std::vector<std::shared_ptr<const mtea::Connection>> out(connections.begin(), connections.end());
    return out;
}

void mtea::to_json(nlohmann::json& j, const ConnectionManager& cm) {
    std::vector<mtea::Connection> conn;

    for (const auto& c : cm.get_connections()) {
        conn.push_back(*c);
    }

    j = conn;
}

void mtea::from_json(const nlohmann::json& j, ConnectionManager& cm) {
    for (const auto& i : j) {
        const auto c = std::make_shared<mtea::Connection>(0, 0, 0, 0);
        from_json(i, *c);
        cm.add_connection(c);
    }
}
