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

#include <sim_math/blocks/base_block.hpp>

namespace sim_math
{

class Limiter : public BaseBlock
{
public:
    const std::string& get_name() const override
    {
        const static std::string s = "LIMITER";
        return s;
    }

    size_t num_inputs() const override
    {
        return 1;
    }

    size_t num_outputs() const override
    {
        return 1;
    }

    void set_input_port(
        const size_t port_num,
        const std::weak_ptr<const Signal> sig) override
    {
        (void)port_num;
        (void)sig;
    }

    std::shared_ptr<Signal> get_output_port(const size_t port_num) const override
    {
        (void)port_num;
        return std::make_shared<DoubleSignal>(0.0);
    }

    std::vector<std::shared_ptr<Parameter>> get_parameter_list() const override
    {
        return {};
    }

    bool check_types() const override
    {
        return true;
    }

    bool emit_code(CodegenState& state) const override
    {
        (void)state;
        return false;
    }
};

}

#endif // TF_SIM_MATH_BLOCK_LIMITER_H
