// SPDX-License-Identifier: GPL-3.0-only

#include "block_interface.hpp"


size_t tmdl::BlockInterface::get_id() const
{
    return _id;
}

void tmdl::BlockInterface::set_id(const size_t id)
{
    _id = id;
}

std::vector<std::shared_ptr<tmdl::Parameter>> tmdl::BlockInterface::get_parameters() const
{
    return {};
}

bool tmdl::BlockInterface::outputs_are_delayed() const
{
    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::BlockInterface::make_error(const std::string& msg) const
{
    return std::make_unique<BlockError>(BlockError
    {
        .id = get_id(),
        .message = msg
    });
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
