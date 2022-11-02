// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_SIM_MATH_BLOCK_LIMITER_H
#define TF_SIM_MATH_BLOCK_LIMITER_H

#include <cstdlib>

#include <vector>
#include <string>
#include <memory>

#include <sim_math/signal.hpp>

#include <sim_math/codegen_state.hpp>
#include <sim_math/data_types.hpp>

#include <sim_math/blocks/block_base.hpp>

namespace sim_math
{

class Limiter : public BaseBlock
{
public:
    size_t num_inputs() const
    {
        return 1;
    }

    size_t num_outputs() const
    {
        return 1;
    }

    std::vector<std::shared_ptr<Signal>> get_output_signals() const
    {
        std::vector<std::shared_ptr<Signal>> outputs;
        for (size_t i = 0; i < num_outputs(); ++i)
        {
            outputs.push_back(std::make_shared<DoubleSignal>(0.0));
        }

        return outputs;
    }

    DataType get_output_port_type(const size_t port_num) const
    {
        return get_output_signals()[port_num]->get_data_type();
    }

    bool check_types() const;

    bool emit_code(CodegenState& state) const;
};

}

#endif // TF_SIM_MATH_BLOCK_LIMITER_H
