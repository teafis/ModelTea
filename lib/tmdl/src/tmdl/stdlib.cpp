// SPDX-License-Identifier: GPL-3.0-only

#include "stdlib.hpp"

#include "blocks/clock.hpp"
#include "blocks/limiter.hpp"
#include "blocks/trig.hpp"
#include "blocks/io_ports.hpp"
#include "blocks/arithmetic.hpp"
#include "blocks/integrator.hpp"
#include "blocks/relational.hpp"
#include "blocks/constant.hpp"

#include "model_exception.hpp"

#include <ranges>

template <typename T>
std::shared_ptr<tmdl::BlockInterface> make_block()
{
    return std::make_shared<T>();
}

tmdl::stdlib::StandardLibrary::StandardLibrary()
{
    block_map = {
        {"limiter", &make_block<Limiter>},
        {"cos", &make_block<TrigCos>},
        {"sin", &make_block<TrigSin>},
        {"clock", &make_block<Clock>},
        {"input", &make_block<InputPort>},
        {"output", &make_block<OutputPort>},
        {"add", &make_block<Addition>},
        {"sub", &make_block<Subtraction>},
        {"mul", &make_block<Multiplication>},
        {"div", &make_block<Division>},
        {"integrator", &make_block<Integrator>},
        {"<", &make_block<LessThan>},
        {"<=", &make_block<LessThanEqual>},
        {">", &make_block<GreaterThan>},
        {">=", &make_block<GreaterThanEqual>},
        {"==", &make_block<Equal>},
        {"!=", &make_block<NotEqual>},
        {"constant", &make_block<Constant>}
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
    std::sort(keys.begin(), keys.end());
    return keys;
}

std::shared_ptr<tmdl::BlockInterface> tmdl::stdlib::StandardLibrary::create_block_from_name(const std::string& name) const
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
