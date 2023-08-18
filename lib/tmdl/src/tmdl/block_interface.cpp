// SPDX-License-Identifier: GPL-3.0-only

#include "block_interface.hpp"

tmdl::BlockLocation::BlockLocation() :
    x{0}, y{0}
{
    // Empty Constructor
}

tmdl::BlockLocation::BlockLocation(const int64_t x, const int64_t y) :
    x{x}, y{y}
{
    // Empty Constructor
}

tmdl::CompiledBlockInterface::~CompiledBlockInterface()
{
    // Empty Destructor
}

std::vector<std::unique_ptr<tmdl::codegen::CodeComponent>> tmdl::CompiledBlockInterface::get_codegen_components() const
{
    std::vector<std::unique_ptr<tmdl::codegen::CodeComponent>> components;
    auto self_block = get_codegen_self();

    if (!self_block->is_virtual())
    {
        components.push_back(std::move(self_block));
    }

    for (auto& c : get_codegen_other())
    {
        if (c->is_virtual())
        {
            continue;
        }

        if (std::find_if(components.begin(), components.end(), [&c](std::unique_ptr<tmdl::codegen::CodeComponent>& x) { return c->get_name_base() == x->get_name_base(); }) == components.end())
        {
            components.push_back(std::move(c));
        }
    }

    return components;
}

std::vector<std::unique_ptr<tmdl::codegen::CodeComponent>> tmdl::CompiledBlockInterface::get_codegen_other() const
{
    return {};
}

tmdl::BlockInterface::BlockInterface() :
    _id(0),
    _loc{}
{
    // Empty Constructor
}

tmdl::BlockInterface::~BlockInterface()
{
    // Empty Destructor
}

tmdl::BlockInterface::ModelInfo::ModelInfo(const double dt) : dt(dt)
{
    // Empty Constructor
}

double tmdl::BlockInterface::ModelInfo::get_dt() const
{
    return dt;
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

tmdl::BlockExecutionInterface::~BlockExecutionInterface()
{
    // Empty Destructor
}

void tmdl::BlockExecutionInterface::init()
{
    // Empty Init
}

void tmdl::BlockExecutionInterface::step()
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
