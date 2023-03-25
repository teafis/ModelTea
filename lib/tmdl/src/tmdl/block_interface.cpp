// SPDX-License-Identifier: GPL-3.0-only

#include "block_interface.hpp"

tmdl::BlockLocation::BlockLocation() :
    x(0), y(0)
{
    // Empty Constructor
}

tmdl::BlockLocation::BlockLocation(const int64_t x, const int64_t y) :
    x(x), y(y)
{
    // Empty Constructor
}

tmdl::BlockInterface::BlockInterface() :
    _id(0),
    _loc{}
{
    // Empty Constructor
}

size_t tmdl::BlockInterface::get_id() const
{
    return _id;
}

void tmdl::BlockInterface::set_id(const size_t id)
{
    _id = id;
}

void tmdl::BlockInterface::set_loc(const BlockLocation& loc)
{
    _loc = loc;
}

const tmdl::BlockLocation& tmdl::BlockInterface::get_loc() const
{
    return _loc;
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

void tmdl::BlockExecutionInterface::post_step()
{
    // Empty post-step
}

void tmdl::BlockExecutionInterface::reset()
{
    // Empty Reset
}

void tmdl::BlockExecutionInterface::close()
{
    // Empty Close
}

std::unique_ptr<tmdl::codegen::CodeComponent> tmdl::BlockExecutionInterface::generate_code_component()
{
    // Not implemented -> TODO - remove and replace with pure virtual
    throw 0;
}
