// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_LIMITER_HPP
#define TF_MODEL_STDLIB_LIMITER_HPP

#include <tmdl/block.hpp>


namespace tmdl
{

namespace stdlib
{

class Limiter : public Block
{
public:
    std::string get_name() const override
    {
        return "limiter";
    }

    std::string get_description() const override
    {
        return "Limits input values by the provided parameters";
    }

    std::vector<Parameter*> get_parameters() const override
    {
        return {};
    }

    size_t get_num_inputs() const override
    {
        return 1;
    }

    size_t get_num_outputs() const override
    {
        return 1;
    }

    /*
    std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override
    {
        return nullptr;
    }
    */
};

}

}

#endif // TF_MODEL_STDLIB_LIMITER_HPP
