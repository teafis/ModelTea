// SPDX-License-Identifier: GPL-3.0-only

#include "delay.hpp"

#include "../model_exception.hpp"

template <typename T>
class DelayExecutor : public tmdl::BlockExecutionInterface
{
public:
    DelayExecutor(
        const std::shared_ptr<const tmdl::ValueBox> input,
        const std::shared_ptr<tmdl::ValueBox> output,
        const std::shared_ptr<const tmdl::ValueBoxType<bool>> reset_flag,
        const std::shared_ptr<const tmdl::ValueBox> reset_value) :
        _input(std::dynamic_pointer_cast<const tmdl::ValueBoxType<T>>(input)),
        _output(std::dynamic_pointer_cast<tmdl::ValueBoxType<T>>(output)),
        _reset_flag(reset_flag),
        _reset_value(std::dynamic_pointer_cast<const tmdl::ValueBoxType<T>>(reset_value))
    {
        if (_input == nullptr || _output == nullptr || _reset_flag == nullptr || _reset_value == nullptr)
        {
            throw tmdl::ModelException("input values must be non-null");
        }
    }

    void init() override
    {
        reset();
    }

    void step(const tmdl::SimState&) override
    {
        // Do Nothing -> Reset?
    }

    void reset() override
    {
        _output->value = _reset_value->value;
    }

    void post_step() override
    {
        _output->value = _input->value;
    }

protected:
    std::shared_ptr<const tmdl::ValueBoxType<T>> _input;
    std::shared_ptr<tmdl::ValueBoxType<T>> _output;
    std::shared_ptr<const tmdl::ValueBoxType<bool>> _reset_flag;
    std::shared_ptr<const tmdl::ValueBoxType<T>> _reset_value;
};

tmdl::stdlib::Delay::Delay()
{
    input_type = DataType::UNKNOWN;
    input_reset_flag = DataType::UNKNOWN;
    input_reset_value = DataType::UNKNOWN;
    output_port = DataType::UNKNOWN;
}

std::string tmdl::stdlib::Delay::get_name() const
{
    return "delay";
}

std::string tmdl::stdlib::Delay::get_description() const
{
    return "delays the provided input value by one cycle";
}

size_t tmdl::stdlib::Delay::get_num_inputs() const
{
    return 3;
}

size_t tmdl::stdlib::Delay::get_num_outputs() const
{
    return 1;
}

bool tmdl::stdlib::Delay::update_block()
{
    if (input_type != output_port)
    {
        output_port = input_type;
        return true;
    }

    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::stdlib::Delay::has_error() const
{
    if (input_type != output_port)
    {
        return make_error("input type does not equal output type");
    }
    else if (input_reset_flag != DataType::BOOLEAN)
    {
        return make_error("reset flag must be boolean");
    }
    else if (input_type != input_reset_value)
    {
        return make_error("input type must equal the reset value type");
    }

    return nullptr;
}

void tmdl::stdlib::Delay::set_input_type(
    const size_t port,
    const DataType type)
{
    switch (port)
    {
    case 0:
        input_type = type;
        break;
    case 1:
        input_reset_flag = type;
        break;
    case 2:
        input_reset_value = type;
        break;
    default:
        throw ModelException("invalid input port provided");
    }
}

tmdl::DataType tmdl::stdlib::Delay::get_output_type(const size_t port) const
{
    if (port == 0)
    {
        return output_port;
    }
    else
    {
        throw ModelException("invalid output port provided");
    }
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Delay::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot build executor with error");
    }

    auto input_value = manager.get_ptr(connections.get_connection_to(get_id(), 0));

    auto input_value_reset_flag = std::dynamic_pointer_cast<const ValueBoxType<bool>>(manager.get_ptr(connections.get_connection_to(get_id(), 1)));
    auto input_value_reset_value = manager.get_ptr(connections.get_connection_to(get_id(), 2));

    auto output_value = manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (output_port)
    {
    case DataType::DOUBLE:
        return std::make_shared<DelayExecutor<double>>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    case DataType::SINGLE:
        return std::make_shared<DelayExecutor<float>>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    case DataType::BOOLEAN:
        return std::make_shared<DelayExecutor<bool>>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    case DataType::INT32:
        return std::make_shared<DelayExecutor<int32_t>>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    case DataType::UINT32:
        return std::make_shared<DelayExecutor<uint32_t>>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    default:
        throw ModelException("unknown data type provided");
    }
}
