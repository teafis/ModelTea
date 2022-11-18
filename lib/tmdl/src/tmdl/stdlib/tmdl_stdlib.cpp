// SPDX-License-Identifier: GPL-3.0-only

#include "tmdl_stdlib.hpp"

#include <tmdl/stdlib/clock.hpp>
#include <tmdl/stdlib/limiter.hpp>
#include <tmdl/stdlib/trig.hpp>

#include <tmdl/model_exception.hpp>

#include <ranges>

template <typename T>
std::shared_ptr<tmdl::LibraryBlock> make_block()
{
    return std::make_shared<T>();
}


tmdl::stdlib::StandardLibrary::StandardLibrary()
{
    block_map = {
        {"limiter", &make_block<Limiter>},
        {"cos", &make_block<TrigCos>},
        {"sin", &make_block<TrigSin>},
        {"clock", &make_block<Clock>}
    };
}

bool tmdl::stdlib::StandardLibrary::has_block(const std::string& name) const
{
    const auto it = block_map.find(name);
    return it != block_map.end();
}

std::string tmdl::stdlib::StandardLibrary::get_library_name() const
{
    return "stdlib";
}

std::vector<std::string> tmdl::stdlib::StandardLibrary::get_block_names() const
{
    std::vector<std::string> keys;
    for (const auto& i : block_map)
    {
        keys.push_back(i.first);
    }
    return keys;
}

std::shared_ptr<tmdl::LibraryBlock> tmdl::stdlib::StandardLibrary::create_block_from_name(const std::string& name) const
{
    auto it = block_map.find(name);

    if (it != block_map.end())
    {
        return it->second();
    }
    else
    {
        throw ModelException("unknown block type provided to library");
    }
}
