// SPDX-License-Identifier: GPL-3.0-only

#include "stdlib.hpp"

#include "../blocks/arithmetic.hpp"
#include "../blocks/clock.hpp"
#include "../blocks/constant.hpp"
#include "../blocks/delay.hpp"
#include "../blocks/derivative.hpp"
#include "../blocks/integrator.hpp"
#include "../blocks/io_ports.hpp"
#include "../blocks/limiter.hpp"
#include "../blocks/relational.hpp"
#include "../blocks/trig.hpp"

#include "../model_exception.hpp"

#include <ranges>

template <typename T>
std::shared_ptr<tmdl::BlockInterface> make_block()
{
    return std::make_shared<T>();
}

tmdl::blocks::StandardLibrary::StandardLibrary()
{
    block_map = {
        {"limiter", &make_block<Limiter>},
        {"cos", &make_block<TrigCos>},
        {"sin", &make_block<TrigSin>},
        {"clock", &make_block<Clock>},
        {"input", &make_block<InputPort>},
        {"output", &make_block<OutputPort>},
        {"+", &make_block<Addition>},
        {"-", &make_block<Subtraction>},
        {"*", &make_block<Multiplication>},
        {"/", &make_block<Division>},
        {"integrator", &make_block<Integrator>},
        {"<", &make_block<LessThan>},
        {"<=", &make_block<LessThanEqual>},
        {">", &make_block<GreaterThan>},
        {">=", &make_block<GreaterThanEqual>},
        {"==", &make_block<Equal>},
        {"!=", &make_block<NotEqual>},
        {"constant", &make_block<Constant>},
        {"delay", &make_block<Delay>},
        {"derivative", &make_block<Derivative>}
    };
}

bool tmdl::blocks::StandardLibrary::has_block(const std::string& name) const
{
    const auto it = block_map.find(name);
    return it != block_map.end();
}

std::string tmdl::blocks::StandardLibrary::get_library_name() const
{
    return "stdlib";
}

std::vector<std::string> tmdl::blocks::StandardLibrary::get_block_names() const
{
    std::vector<std::string> keys;
    for (const auto& i : block_map)
    {
        keys.push_back(i.first);
    }
    std::sort(keys.begin(), keys.end());
    return keys;
}

bool tmdl::blocks::StandardLibrary::has_block(const std::string name) const
{
    return block_map.find(name) != block_map.end();
}

std::shared_ptr<tmdl::BlockInterface> tmdl::blocks::StandardLibrary::create_block(const std::string& name) const
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
