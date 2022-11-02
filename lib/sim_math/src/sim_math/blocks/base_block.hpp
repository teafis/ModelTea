// SPDX-License-Identifier: GPL-3.0-only

#ifndef SIM_MATH_BASE_BLOCK_HPP
#define SIM_MATH_BASE_BLOCK_HPP

#include <cstdlib>

#include <memory>
#include <vector>

#include <sim_math/data_types.hpp>
#include <sim_math/codegen_state.hpp>
#include <sim_math/parameter.hpp>

#include <sim_math/signal.hpp>

namespace sim_math
{

class BaseBlock
{
public:
    virtual const std::string& get_name() const = 0;

    virtual size_t num_inputs() const = 0;

    virtual size_t num_outputs() const = 0;

    virtual void set_input_port(
        const size_t port_num,
        const std::weak_ptr<const Signal> sig) = 0;

    virtual std::shared_ptr<Signal> get_output_port(const size_t port_num) const = 0;

    virtual std::vector<std::shared_ptr<Parameter>> get_parameter_list() const = 0;

    virtual bool check_types() const = 0;

    virtual bool emit_code(CodegenState& state) const = 0;
};

}

#endif // SIM_MATH_BASE_BLOCK_HPP
