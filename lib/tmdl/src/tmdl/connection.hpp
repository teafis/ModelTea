// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CONNECTION_HPP
#define TF_MODEL_CONNECTION_HPP

#include <cstdlib>
#include <optional>

#include <nlohmann/json.hpp>

#include "values/identifiers.hpp"


namespace tmdl
{

class Connection;

void to_json(nlohmann::json&, const Connection&);
void from_json(const nlohmann::json&, Connection&);

class Connection
{
public:
    Connection(
        const size_t from_id,
        const size_t from_port,
        const size_t to_id,
        const size_t to_port);

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

#endif // TF_MODEL_CONNECTION_HPP
