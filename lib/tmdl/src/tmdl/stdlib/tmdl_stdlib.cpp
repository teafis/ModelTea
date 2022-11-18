// SPDX-License-Identifier: GPL-3.0-only

#include "tmdl_stdlib.hpp"

#include <tmdl/stdlib/clock.hpp>
#include <tmdl/stdlib/limiter.hpp>
#include <tmdl/stdlib/trig.hpp>

#include <tmdl/model_exception.hpp>


std::string tmdl::stdlib::StandardLibrary::get_library_name() const
{
    return "stdlib";
}

std::vector<std::string> tmdl::stdlib::StandardLibrary::get_block_names() const
{
    return {
        "limiter",
        "cos",
        "sin",
        "clock"
    };
}

std::shared_ptr<tmdl::Block> tmdl::stdlib::StandardLibrary::create_block_from_name(const std::string& name) const
{
    if (name == "limiter")
    {
        return std::make_shared<Limiter>();
    }
    else if (name == "cos")
    {
        return std::make_shared<TrigCos>();
    }
    else if (name == "sin")
    {
        return std::make_shared<TrigSin>();
    }
    else if (name == "clock")
    {
        return std::make_shared<Clock>();
    }
    else
    {
        throw ModelException("unknown block type provided to library");
    }
}
