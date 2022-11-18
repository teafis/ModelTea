// SPDX-License-Identifier: GPL-3.0-only

#include "model_exception.hpp"

tmdl::ModelException::ModelException(const std::string& msg) : _msg(msg)
{
    // Empty Constructor
}

const std::string& tmdl::ModelException::what() const
{
    return _msg;
}

tmdl::ModelException::~ModelException()
{
    // Empty Destructor
}


tmdl::ExecutionException::ExecutionException(
    const std::string& msg,
    const size_t id) :
    ModelException(msg),
    _id(id)
{
    // Empty Constructor
}

size_t tmdl::ExecutionException::get_id() const
{
    return _id;
}
