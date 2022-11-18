// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/stdlib/limiter.hpp>

using namespace tmdl;
using namespace tmdl::stdlib;


template <typename T>
class LimiterExecutor : public BlockExecutionInterface
{
public:
    LimiterExecutor(
        const T* ptr_input,
        const T* val_min,
        const T* val_max,
        T* ptr_output) :
        _ptr_input(ptr_input),
        _ptr_output(ptr_output),
        _val_min(val_min),
        _val_max(val_max)
    {
        if (ptr_input == nullptr || ptr_output == nullptr)
        {
            throw ModelException("input pointers cannot be null");
        }
        else if (val_max < val_min)
        {
            throw ModelException("maximum value must be above the minimum value");
        }
    }

public:
    void step(const SimState&) override
    {
        const T current = *_ptr_input;
        if (current < *_val_min)
        {
            *_ptr_output = *_val_min;
        }
        else if (current > *_val_max)
        {
            *_ptr_output = *_val_max;
        }
        else
        {
            *_ptr_output = current;
        }
    }

protected:
    const T* _ptr_input;
    T* _ptr_output;
    const T* _val_min;
    const T* _val_max;
};

Limiter::Limiter()
{
    input_port_value = nullptr;
    output_port_value = nullptr;

    output_port = std::make_unique<PortValue>(
    PortValue {
        .dtype = DataType::UNKNOWN,
        .ptr = nullptr
    });

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
    if (input_port_value == nullptr)
    {
        if (output_port->dtype != DataType::UNKNOWN)
        {
            output_port->dtype = DataType::UNKNOWN;
            output_port->ptr = nullptr;
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (input_port_value->dtype != output_port->dtype)
    {
        output_port_value = nullptr;
        output_port->ptr = nullptr;
        output_port->dtype = DataType::UNKNOWN;

        ParameterValue::Type newPrmType = ParameterValue::Type::UNKNOWN;

        switch (input_port_value->dtype)
        {
        case DataType::DOUBLE:
            newPrmType = ParameterValue::Type::DOUBLE;
            output_port_value = std::make_unique<ValueBoxType<double>>(0.0);
            break;
        case DataType::INT32:
            newPrmType = ParameterValue::Type::INT32;
            output_port_value = std::make_unique<ValueBoxType<int32_t>>(0);
            break;
        default:
            break;
        }

        prmMinValue->get_value().convert(newPrmType);
        prmMaxValue->get_value().convert(newPrmType);

        if (output_port_value != nullptr)
        {
            output_port->dtype = input_port_value->dtype;
            output_port->ptr = output_port_value->get_ptr_val();
        }

        return true;
    }
    else
    {
        return false;
    }
}

std::unique_ptr<const BlockError> Limiter::has_error() const
{
    if (input_port_value == nullptr)
    {
        return std::make_unique<BlockError>(BlockError
        {
            .id = get_id(),
            .message = "input port not set"
        });
    }
    else if (output_port_value == nullptr)
    {
        return std::make_unique<BlockError>(BlockError
        {
            .id = get_id(),
            .message = "invalid output value set"
        });
    }
    else if (!dynamicLimiter->get_value().value.tf)
    {
        if (prmMaxValue == nullptr || prmMinValue == nullptr)
        {
            return std::make_unique<BlockError>(BlockError
            {
                .id = get_id(),
                .message = "min or max value is not able to be set"
            });
        }
    }

    return nullptr;
}

void Limiter::set_input_port(
    const size_t port,
    const PortValue* value)
{
    if (port < get_num_inputs())
    {
        input_port_value = value;
    }
    else
    {
        throw ModelException("provided input port too high");
    }
}

const PortValue* Limiter::get_output_port(const size_t port) const
{
    if (port < get_num_outputs())
    {
        return output_port.get();
    }
    else
    {
        throw ModelException("provided output port is too high");
    }
}

std::shared_ptr<BlockExecutionInterface> Limiter::get_execution_interface() const
{
    if (input_port_value == nullptr || output_port_value == nullptr)
    {
        throw ModelException("cannot execute with incomplete input parameters");
    }

    switch (input_port_value->dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<LimiterExecutor<double>>(
            reinterpret_cast<const double*>(input_port_value->ptr),
            &prmMinValue->get_value().value.f64,
            &prmMaxValue->get_value().value.f64,
            reinterpret_cast<double*>(output_port_value->get_ptr_val()));
    case DataType::SINGLE:
        return std::make_shared<LimiterExecutor<float>>(
            reinterpret_cast<const float*>(input_port_value->ptr),
            &prmMinValue->get_value().value.f32,
            &prmMaxValue->get_value().value.f32,
            reinterpret_cast<float*>(output_port_value->get_ptr_val()));
    case DataType::INT32:
        return std::make_shared<LimiterExecutor<int32_t>>(
            reinterpret_cast<const int32_t*>(input_port_value->ptr),
            &prmMinValue->get_value().value.i32,
            &prmMaxValue->get_value().value.i32,
            reinterpret_cast<int32_t*>(output_port_value->get_ptr_val()));
    case DataType::UINT32:
        return std::make_shared<LimiterExecutor<uint32_t>>(
            reinterpret_cast<const uint32_t*>(input_port_value->ptr),
            &prmMinValue->get_value().value.u32,
            &prmMaxValue->get_value().value.u32,
            reinterpret_cast<uint32_t*>(output_port_value->get_ptr_val()));
    default:
        throw ModelException("unable to generate limitor executor");
    }
}
