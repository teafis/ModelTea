// SPDX-License-Identifier: GPL-3.0-only

#include "derivative.hpp"

#include "../model_exception.hpp"


template <typename T>
class DerivativeExecutor : public tmdl::BlockExecutionInterface
{
public:
    DerivativeExecutor(
        std::shared_ptr<const tmdl::ValueBox> input,
        std::shared_ptr<tmdl::ValueBox> output,
        std::shared_ptr<const tmdl::ValueBoxType<bool>> reset_flag) :
        _input(std::dynamic_pointer_cast<const tmdl::ValueBoxType<T>>(input)),
        _reset_flag(reset_flag),
        _output(std::dynamic_pointer_cast<tmdl::ValueBoxType<T>>(output))
    {
        if (_input == nullptr || _output == nullptr || _reset_flag == nullptr)
        {
            throw tmdl::ModelException("input parameters are null");
        }
    }

    void init() override
    {
        reset();
    }

    void step(const tmdl::SimState& s) override
    {
        if (_reset_flag->value)
        {
            reset();
        }

        _output->value = (_input->value - last_val) / s.dt;
        last_val = _input->value;
    }

    void reset() override
    {
        last_val = _input->value;
    }

protected:
    std::shared_ptr<const tmdl::ValueBoxType<T>> _input;
    std::shared_ptr<const tmdl::ValueBoxType<bool>> _reset_flag;
    std::shared_ptr<tmdl::ValueBoxType<T>> _output;
    T last_val;
};


tmdl::stdlib::Derivative::Derivative()
{
    input_type = DataType::UNKNOWN;
    input_reset_flag_type = DataType::UNKNOWN;
}

std::string tmdl::stdlib::Derivative::get_name() const
{
    return "derivative";
}

std::string tmdl::stdlib::Derivative::get_description() const
{
    return "derives the provided values";
}

size_t tmdl::stdlib::Derivative::get_num_inputs() const
{
    return 2;
}

size_t tmdl::stdlib::Derivative::get_num_outputs() const
{
    return 1;
}

bool tmdl::stdlib::Derivative::update_block()
{
    if (output_port.dtype != input_type)
    {
        output_port.dtype = input_type;
        return true;
    }

    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::stdlib::Derivative::has_error() const
{
    if (input_type != DataType::DOUBLE && input_type != DataType::SINGLE)
    {
        return make_error("integrator only works with floating point types");
    }
    if (input_type != output_port.dtype)
    {
        return make_error("input port doesn't match output port type");
    }
    else if (input_reset_flag_type != DataType::BOOLEAN)
    {
        return make_error("reset flag must be a boolean type");
    }

    return nullptr;
}

void tmdl::stdlib::Derivative::set_input_port(
    const size_t port,
    const DataType type)
{
    switch (port)
    {
    case 0:
        input_type = type;
        break;
    case 1:
        input_reset_flag_type = type;
        break;
    default:
        throw ModelException("input port out of range");
    }
}

tmdl::PortValue tmdl::stdlib::Derivative::get_output_port(const size_t port) const
{
    if (port == 0)
    {
        return output_port;
    }
    else
    {
        throw ModelException("output port out of range");
    }
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Derivative::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    const auto err = has_error();
    if (err != nullptr)
    {
        throw ModelException("cannot creator interface with an error");
    }

    auto in_value = manager.get_ptr(connections.get_connection_to(get_id(), 0));
    auto in_reset_flag = std::dynamic_pointer_cast<const ValueBoxType<bool>>(manager.get_ptr(connections.get_connection_to(get_id(), 1)));

    auto out_value = manager.get_ptr(VariableIdentifier {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (input_type)
    {
    case DataType::DOUBLE:
        return std::make_shared<DerivativeExecutor<double>>(in_value, out_value, in_reset_flag);
    case DataType::SINGLE:
        return std::make_shared<DerivativeExecutor<float>>(in_value, out_value, in_reset_flag);
    default:
        throw ModelException("unable to create pointer value");
    }
}
