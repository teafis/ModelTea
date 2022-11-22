// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/stdlib/limiter.hpp>

#include <algorithm>

using namespace tmdl;
using namespace tmdl::stdlib;


template <typename T>
class LimiterExecutor : public BlockExecutionInterface
{
public:
    LimiterExecutor(
        std::shared_ptr<const ValueBox> ptr_input,
        std::shared_ptr<const ValueBox> val_min,
        std::shared_ptr<const ValueBox> val_max,
        std::shared_ptr<ValueBox> ptr_output) :
        _ptr_input(std::dynamic_pointer_cast<const ValueBoxType<T>>(ptr_input)),
        _ptr_output(std::dynamic_pointer_cast<ValueBoxType<T>>(ptr_output)),
        _val_min(std::dynamic_pointer_cast<const ValueBoxType<T>>(val_min)),
        _val_max(std::dynamic_pointer_cast<const ValueBoxType<T>>(val_max))
    {
        if (_ptr_input == nullptr || _ptr_output == nullptr || _val_min == nullptr || _val_max == nullptr)
        {
            throw ModelException("input pointers cannot be null");
        }
    }

public:
    void step(const SimState&) override
    {
        const T current = _ptr_input->value;
        if (current < _val_min->value)
        {
            _ptr_output->value = _val_min->value;
        }
        else if (current > _val_max->value)
        {
            _ptr_output->value = _val_max->value;
        }
        else
        {
            _ptr_output->value = current;
        }
    }

protected:
    std::shared_ptr<const ValueBoxType<T>> _ptr_input;
    std::shared_ptr<ValueBoxType<T>> _ptr_output;
    std::shared_ptr<const ValueBoxType<T>> _val_min;
    std::shared_ptr<const ValueBoxType<T>> _val_max;
};

Limiter::Limiter()
{
    // Setup parameters
    dynamicLimiter = std::make_unique<Parameter>(
        "dynamic_limiter",
        "Use Dynamic Limits",
        ParameterValue
        {
            .dtype = ParameterValue::Type::BOOLEAN,
            .value = ParameterValue::Value
            {
                .tf = false
            }
        });

    prmMaxValue = std::make_unique<Parameter>(
        "max_value",
        "Maximum Value",
        ParameterValue{});

    prmMinValue = std::make_unique<Parameter>(
        "min_value",
        "Minimum Value",
        ParameterValue{});

    Limiter::update_block();
}

std::string Limiter::get_name() const
{
    return "limiter";
}

std::string Limiter::get_description() const
{
    return "Limits input values by the provided parameters";
}

std::vector<Parameter*> Limiter::get_parameters() const
{
    if (dynamicLimiter->get_value().value.tf)
    {
        return {
            dynamicLimiter.get()
        };
    }
    else
    {
        return {
            dynamicLimiter.get(),
            prmMinValue.get(),
            prmMaxValue.get()
        };
    }
}

size_t Limiter::get_num_inputs() const
{
    if (dynamicLimiter->get_value().value.tf)
    {
        return 3;
    }
    else
    {
        return 1;
    }
}

size_t Limiter::get_num_outputs() const
{
    return 1;
}

bool Limiter::update_block()
{
    if (input_port.dtype != output_port.dtype)
    {
        ParameterValue::Type new_dtype = ParameterValue::Type::UNKNOWN;

        switch (input_port.dtype)
        {
        case DataType::DOUBLE:
            new_dtype = ParameterValue::Type::DOUBLE;
            break;
        case DataType::SINGLE:
            new_dtype = ParameterValue::Type::SINGLE;
            break;
        case DataType::UINT32:
            new_dtype = ParameterValue::Type::UINT32;
            break;
        case DataType::INT32:
            new_dtype = ParameterValue::Type::INT32;
            break;
        default:
            break;
        }

        prmMaxValue->get_value().convert(new_dtype);
        prmMinValue->get_value().convert(new_dtype);


        output_port = input_port;
        return true;
    }

    return false;
}

std::unique_ptr<const BlockError> Limiter::has_error() const
{
    std::vector<DataType> supportedDataTypes = {
        DataType::DOUBLE,
        DataType::SINGLE,
        DataType::INT32,
        DataType::UINT32
    };

    if (std::find(supportedDataTypes.begin(), supportedDataTypes.end(), input_port.dtype) == supportedDataTypes.end())
    {
        return std::make_unique<BlockError>(BlockError
        {
            .id = get_id(),
            .message = "input port not set"
        });
    }

    if (dynamicLimiter->get_value().value.tf)
    {
        if (input_port_max.dtype != input_port.dtype || input_port_min.dtype != input_port.dtype)
        {
            return std::make_unique<BlockError>(BlockError
            {
                .id = get_id(),
                .message = "min/max data types should be the same data type as the primary input"
            });
        }
    }
    else
    {
        for (const auto& p : get_parameters())
        {
            if (p->get_value().dtype == ParameterValue::Type::UNKNOWN)
            {
                return std::make_unique<BlockError>(BlockError
                {
                    .id = get_id(),
                    .message = "min or max value is not able to be set"
                });
            }
        }
    }

    return nullptr;
}

void Limiter::set_input_port(
    const size_t port,
    const PortValue value)
{
    if (port < get_num_inputs())
    {
        switch (port)
        {
        case 0:
            input_port = value;
            break;
        case 1:
            input_port_min = value;
            break;
        case 2:
            input_port_max = value;
            break;
        }
    }
    else
    {
        throw ModelException("provided input port too high");
    }
}

PortValue Limiter::get_output_port(const size_t port) const
{
    if (port < get_num_outputs())
    {
        return output_port;
    }
    else
    {
        throw ModelException("provided output port is too high");
    }
}

std::shared_ptr<BlockExecutionInterface> Limiter::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot execute with incomplete input parameters");
    }

    std::shared_ptr<ValueBox> maxValue;
    std::shared_ptr<ValueBox> minValue;

    if (dynamicLimiter->get_value().value.tf)
    {
        maxValue = manager.get_ptr(connections.get_connection_to(get_id(), 1));
        minValue = manager.get_ptr(connections.get_connection_to(get_id(), 2));
    }
    else
    {
        maxValue = prmMaxValue->get_value().to_box();
        minValue = prmMinValue->get_value().to_box();
    }

    const auto inputPointer = manager.get_ptr(connections.get_connection_to(get_id(), 0));

    const auto vidOutput = VariableIdentifier {
        .block_id = get_id(),
        .output_port_num = 0
    };

    const auto outputPointer = manager.get_ptr(vidOutput);

    switch (input_port.dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<LimiterExecutor<double>>(
            inputPointer,
            minValue,
            maxValue,
            outputPointer);
    case DataType::SINGLE:
        return std::make_shared<LimiterExecutor<float>>(
            inputPointer,
            minValue,
            maxValue,
            outputPointer);
    case DataType::INT32:
        return std::make_shared<LimiterExecutor<int32_t>>(
            inputPointer,
            minValue,
            maxValue,
            outputPointer);
    case DataType::UINT32:
        return std::make_shared<LimiterExecutor<uint32_t>>(
            inputPointer,
            minValue,
            maxValue,
            outputPointer);
    default:
        throw ModelException("unable to generate limitor executor");
    }
}
