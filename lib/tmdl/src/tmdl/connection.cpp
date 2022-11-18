// SPDX-License-Identifier: GPL-3.0-only

#include "connection.hpp"

tmdl::Connection::Connection(
    const size_t from_id,
    const size_t from_port,
    const size_t to_id,
    const size_t to_port) :
    from_id(from_id),
    from_port(from_port),
    to_id(to_id),
    to_port(to_port)
{
    // Empty Constructor
}

bool tmdl::Connection::contains_id(const size_t id) const
{
    return from_id == id || to_id == id;
}

size_t tmdl::Connection::get_from_id() const
{
    return from_id;
}

size_t tmdl::Connection::get_from_port() const
{
    return from_port;
}

size_t tmdl::Connection::get_to_id() const
{
    return to_id;
}

size_t tmdl::Connection::get_to_port() const
{
    return to_port;
}

bool tmdl::Connection::operator==(const Connection& other) const
{
    return
        from_id == other.from_id &&
        from_port == other.from_port &&
        to_id == other.to_id &&
        to_port == other.to_port;
}
