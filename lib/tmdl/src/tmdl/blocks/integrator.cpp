// SPDX-License-Identifier: GPL-3.0-only

#include "integrator.hpp"

#include "../model_exception.hpp"

#include <concepts>

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <Number T>
struct IntegratorExecutor : public tmdl::BlockExecutionInterface
{
    IntegratorExecutor(
        std::shared_ptr<const tmdl::ValueBox> input,
        std::shared_ptr<const tmdl::ValueBox> reset_value,
        std::shared_ptr<const tmdl::ValueBoxType<bool>> reset_flag,
        std::shared_ptr<tmdl::ValueBox> output) :
        _input(std::dynamic_pointer_cast<const tmdl::ValueBoxType<T>>(input)),
        _reset_value(std::dynamic_pointer_cast<const tmdl::ValueBoxType<T>>(reset_value)),
        _output(std::dynamic_pointer_cast<tmdl::ValueBoxType<T>>(output)),
        _reset_flag(reset_flag)
    {
        if (_input == nullptr || _reset_value == nullptr || _reset_flag == nullptr || _output == nullptr)
        {
            throw tmdl::ModelException("all pointers must be non-null");
        }
    }

    void step(const tmdl::SimState& state) override
    {
        if (_reset_flag->value)
        {
            reset();
            _output->value = state_value;
        }
        else
        {
            _output->value = state_value;
            state_value += _input->value * state.dt;
        }
    }

    void reset() override
    {
        state_value = _reset_value->value;
    }

protected:
    std::shared_ptr<const tmdl::ValueBoxType<T>> _input;
    std::shared_ptr<const tmdl::ValueBoxType<T>> _reset_value;
    std::shared_ptr<tmdl::ValueBoxType<T>> _output;

    std::shared_ptr<const tmdl::ValueBoxType<bool>> _reset_flag;

    T state_value;
};

tmdl::stdlib::Integrator::Integrator()
{
    input_type = DataType::UNKNOWN;
    input_reset_flag_type = DataType::UNKNOWN;
    input_reset_value_type = DataType::UNKNOWN;

    output_port.is_delayed_output = true;
}

std::string tmdl::stdlib::Integrator::get_name() const
{
    return "integrator";
}

std::string tmdl::stdlib::Integrator::get_description() const
{
    return "integrates the provided values, or resets to the provided value";
}

size_t tmdl::stdlib::Integrator::get_num_inputs() const
{
    return 3;
}

size_t tmdl::stdlib::Integrator::get_num_outputs() const
{
    return 1;
}

bool tmdl::stdlib::Integrator::update_block()
{
    if (output_port.dtype != input_type)
    {
        output_port.dtype = input_type;
        return true;
    }

    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::stdlib::Integrator::has_error() const
{
    if (input_type != DataType::DOUBLE && input_type != DataType::SINGLE)
    {
        return make_error("integrator only works with floating point types");
    }
    if (input_type != output_port.dtype)
    {
        return make_error("input port doesn't match output port type");
    }
    else if (input_type != input_reset_value_type)
    {
        return make_error("input port value and reset value types don't match");
    }
    else if (input_reset_flag_type != DataType::BOOLEAN)
    {
        return make_error("reset flag must be a boolean type");
    }

    return nullptr;
}

void tmdl::stdlib::Integrator::set_input_port(
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
    case 2:
        input_reset_value_type = type;
        break;
    default:
        throw ModelException("input port out of range");
    }
}

tmdl::PortValue tmdl::stdlib::Integrator::get_output_port(const size_t port) const
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

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Integrator::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    const auto err = has_error();
    if (err != nullptr)
    {
        throw ModelException("cannot creator interface with an error");
    }

    const auto in_value = manager.get_ptr(connections.get_connection_to(get_id(), 0));
    const auto in_reset_flag = std::dynamic_pointer_cast<ValueBoxType<bool>>(manager.get_ptr(connections.get_connection_to(get_id(), 1)));
    const auto in_reset_value = manager.get_ptr(connections.get_connection_to(get_id(), 2));

    const auto out_value = manager.get_ptr(VariableIdentifier {
        .block_id = get_id(),
        .output_port_num = 0
    });

    std::shared_ptr<BlockExecutionInterface> exec;

    switch (input_type)
    {
    case DataType::DOUBLE:
        exec = std::make_shared<IntegratorExecutor<double>>(in_value, in_reset_value, in_reset_flag, out_value);
        break;
    case DataType::SINGLE:
        exec = std::make_shared<IntegratorExecutor<float>>(in_value, in_reset_value, in_reset_flag, out_value);
        break;
    default:
        throw ModelException("unable to create pointer value");
    }

    return exec;
}
