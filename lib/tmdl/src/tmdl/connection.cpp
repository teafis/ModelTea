// SPDX-License-Identifier: GPL-3.0-only

module;

#include <cstdlib>
#include <optional>

#include <nlohmann/json.hpp>

export module tmdl:connection;

import :identifier;

namespace tmdl {

class Connection;

void to_json(nlohmann::json&, const Connection&);
void from_json(const nlohmann::json&, Connection&);

class Connection {
public:
    Connection(const size_t from_id, const size_t from_port, const size_t to_id, const size_t to_port);

    bool contains_id(const size_t id) const;

    size_t get_from_id() const;

    size_t get_from_port() const;

    size_t get_to_id() const;

    size_t get_to_port() const;

    const std::optional<Identifier>& get_name() const;

    void set_name(std::string_view n);

    bool operator==(const Connection& other) const = default;

private:
    size_t from_id;
    size_t from_port;
    size_t to_id;
    size_t to_port;
    std::optional<Identifier> name;

public:
    friend void from_json(const nlohmann::json&, tmdl::Connection&);
};

}

tmdl::Connection::Connection(const size_t from_id, const size_t from_port, const size_t to_id, const size_t to_port)
    : from_id(from_id), from_port(from_port), to_id(to_id), to_port(to_port) {
    // Empty Constructor
}

bool tmdl::Connection::contains_id(const size_t id) const { return from_id == id || to_id == id; }

size_t tmdl::Connection::get_from_id() const { return from_id; }

size_t tmdl::Connection::get_from_port() const { return from_port; }

size_t tmdl::Connection::get_to_id() const { return to_id; }

size_t tmdl::Connection::get_to_port() const { return to_port; }

const std::optional<tmdl::Identifier>& tmdl::Connection::get_name() const { return name; }

void tmdl::Connection::set_name(const std::string_view n) { name = Identifier(n); }

void tmdl::to_json(nlohmann::json& j, const tmdl::Connection& c) {
    j["to_block"] = c.get_to_id();
    j["to_port"] = c.get_to_port();
    j["from_block"] = c.get_from_id();
    j["from_port"] = c.get_from_port();

    if (const auto& n = c.get_name(); n.has_value()) {
        j["name"] = n->get();
    }
}

void tmdl::from_json(const nlohmann::json& j, tmdl::Connection& c) {
    j.at("to_block").get_to(c.to_id);
    j.at("to_port").get_to(c.to_port);
    j.at("from_block").get_to(c.from_id);
    j.at("from_port").get_to(c.from_port);

    const auto n = j.find("name");
    if (n != j.end()) {
        std::string name;
        j.at("name").get_to(name);
        c.set_name(name);
    }
}
