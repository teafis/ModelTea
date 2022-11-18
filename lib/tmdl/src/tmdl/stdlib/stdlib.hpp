// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_HPP
#define TF_MODEL_STDLIB_HPP

#include <tmdl/library.hpp>
#include <tmdl/stdlib/clock.hpp>
#include <tmdl/stdlib/limiter.hpp>
#include <tmdl/stdlib/trig.hpp>

#include <tmdl/model_exception.hpp>


namespace tmdl
{

namespace stdlib
{

class StandardLibrary : public LibraryBase
{
public:
    std::string get_library_name() const override
    {
        return "stdlib";
    }

    std::vector<std::string> get_block_names() const override
    {
        return {
            "limiter"
        };
    }

    std::shared_ptr<Block> create_block_from_name(const std::string& name) const override
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
};

}

}

#endif // TF_MODEL_STDLIB_HPP
