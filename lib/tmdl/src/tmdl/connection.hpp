// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CONNECTION_HPP
#define TF_MODEL_CONNECTION_HPP

#include <cstdlib>

namespace tmdl
{

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

    bool operator==(const Connection& other) const = default;

protected:
    size_t from_id;
    size_t from_port;
    size_t to_id;
    size_t to_port;
};

}

#endif // TF_MODEL_CONNECTION_HPP
