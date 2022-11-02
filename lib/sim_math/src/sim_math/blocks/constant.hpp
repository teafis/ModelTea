// SPDX-License-Identifier: GPL-3.0-only

#ifndef SIM_MATH_CONSTANT_BLOCK_HPP
#define SIM_MATH_CONSTANT_BLOCK_HPP

#include <sim_math/blocks/base_block.hpp>

namespace sim_math
{

class ConstantBlockDouble : public BaseBlock
{
public:
    const std::string& get_name() const override
    {
        static const std::string name = "tsim.constant.double";
        return name;
    }

    size_t num_inputs() const override
    {
        return 0;
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
        throw 1;
    }

    std::shared_ptr<Signal> get_output_port(const size_t port_num) const override
    {
        if (port_num == 0)
        {
            return constant_value->get_parameter_value();
        }
        else
        {
            throw 1;
        }
    }

    std::vector<std::shared_ptr<Parameter>> get_parameter_list() const override
    {
        return {
            constant_value
        };
    }

    bool check_types() const override
    {
        return constant_value->get_parameter_value()->get_data_type() == DataType::DOUBLE;
    }

    bool emit_code(CodegenState&) const override
    {
        return false;
    }

private:
    std::shared_ptr<Parameter> constant_value;
};

}

#endif // SIM_MATH_CONSTANT_BLOCK_HPP
