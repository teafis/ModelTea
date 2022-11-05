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
    Limiter()
    {
        prm_dynamic_limiter = std::make_shared<Parameter>(
            "is_dynamic_limit",
            "Uses Dynamic Limiter",
            "If true, the minimum and maximum values are specified by inputs",
            DataType::BOOLEAN,
            "false",
            true);

        prm_maximum_value = std::make_shared<Parameter>(
            "maximum_limit",
            "Maximum Parameter Value",
            "Provides the maximum value allowed as an output",
            DataType::DOUBLE);
        prm_minimum_value = std::make_shared<Parameter>(
            "minimum_limit",
            "Minimum Parameter Value",
            "Provides the minimum value allowed as an output",
            DataType::DOUBLE);

        input_max_val = std::make_shared<PortSignal>("max");
        input_min_val = std::make_shared<PortSignal>("min");
        input_primary = std::make_shared<PortSignal>("x");

        output_primary = std::make_shared<PortSignal>("y");
    }

    const std::string& get_name() const override
    {
        const static std::string s = "LIMITER";
        return s;
    }

    size_t num_inputs() const override
    {
        if (is_dynamic_limit())
        {
            return 3;
        }
        else
        {
            return 1;
        }
    }

    size_t num_outputs() const override
    {
        return 1;
    }

    std::unique_ptr<ComputeResult> set_input_port_signal(
        const size_t port_num,
        const std::weak_ptr<const Signal> sig) override
    {
        if (auto sp = sig.lock(); port_num == 0 && sp)
        {
            // Define the min/max value parameter data types
            prm_maximum_value->set_data_type(sp->get_data_type());
            prm_minimum_value->set_data_type(sp->get_data_type());

            // Create a new output data type with the same type as the input type
            switch (sp->get_data_type())
            {
            case DataType::BOOLEAN:
                output_parameter = std::make_shared<BooleanSignal>();
                break;
            case DataType::DOUBLE:
                output_parameter = std::make_shared<DoubleSignal>();
                break;
            case DataType::INT32:
                output_parameter = std::make_shared<Int32Signal>();
                break;
            case DataType::INT64:
                output_parameter = std::make_shared<Int64Signal>();
                break;
            default:
                return std::make_unique<ComputeError>("incompatible signal type provided");
            }

            output_primary->set_signal(output_parameter);

            // Return success
            return std::make_unique<ComputeSuccess>();
        }
        else
        {
            return std::make_unique<ComputeError>("incorrect port number provided");
        }
    }

    std::weak_ptr<const PortSignal> get_input_port(const size_t port_num) const override
    {
        if (is_dynamic_limit())
        {
            if (port_num == 0)
            {
                return input_primary;
            }
            else
            {
                return {};
            }
        }
        else
        {
            if (port_num == 0)
            {
                return input_max_val;
            }
            else if (port_num == 1)
            {
                return input_primary;
            }
            else if (port_num == 2)
            {
                return input_min_val;
            }
            else
            {
                return {};
            }
        }
    }

    std::weak_ptr<const PortSignal> get_output_port(const size_t port_num) const override
    {
        if (port_num == 0)
        {
            return output_primary;
        }
        else
        {
            return {};
        }
    }

    std::vector<std::weak_ptr<Parameter>> get_parameter_list() const override
    {
        if (is_dynamic_limit())
        {
            return {
                prm_dynamic_limiter
            };
        }
        else
        {
            return {
                prm_dynamic_limiter,
                prm_maximum_value,
                prm_minimum_value
            };
        }
    }

    std::unique_ptr<ComputeResult> compute_step() override
    {
        if (auto r = check_types(); r)
        {
            return std::make_unique<ComputeError>("mismatch in expected types");
        }
        else
        {
            std::make_unique<ComputeError>("unable to check types");
        }

        return std::make_unique<ComputeSuccess>();
    }

    std::unique_ptr<ComputeResult> check_types() const override
    {
        const auto x = input_primary->get_signal().lock();



        return std::make_unique<ComputeSuccess>();
    }

protected:
    bool is_dynamic_limit() const
    {
        const auto dyn_sig = std::dynamic_pointer_cast<BooleanSignal>(prm_dynamic_limiter->get_parameter_value());
        return dyn_sig && dyn_sig->value;
    }

protected:
    std::shared_ptr<Parameter> prm_dynamic_limiter;
    std::shared_ptr<Parameter> prm_maximum_value;
    std::shared_ptr<Parameter> prm_minimum_value;

    std::shared_ptr<Signal> output_parameter;

    std::shared_ptr<PortSignal> input_primary;
    std::shared_ptr<PortSignal> input_min_val;
    std::shared_ptr<PortSignal> input_max_val;

    std::shared_ptr<PortSignal> output_primary;
};

}

#endif // TF_SIM_MATH_BLOCK_LIMITER_H
