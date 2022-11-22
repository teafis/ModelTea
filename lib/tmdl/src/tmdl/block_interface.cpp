// SPDX-License-Identifier: GPL-3.0-only

#include "block_interface.hpp"

#include "model_exception.hpp"


size_t tmdl::BlockInterface::get_id() const
{
    return _id;
}

void tmdl::BlockInterface::set_id(const size_t id)
{
    _id = id;
}

bool tmdl::BlockInterface::is_delayed_input(const size_t port) const
{
    if (port < get_num_inputs())
    {
        return false;
    }
    else
    {
        throw ModelException("port number exceeds input port count");
    }
}


void tmdl::BlockExecutionInterface::init()
{
    // Empty Init
}

void tmdl::BlockExecutionInterface::step(const SimState&)
{
    // Empty Step
}

void tmdl::BlockExecutionInterface::reset()
{
    // Empty Reset
}

void tmdl::BlockExecutionInterface::close()
{
    // Empty Close
}
