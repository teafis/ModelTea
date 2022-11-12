#include "limiter.hpp"

using namespace tmdl;
using namespace tmdl::stdlib;


template <typename T>
class LimiterExecutor : public BlockExecutionInterface
{
public:
    LimiterExecutor(
        const T* ptr_input,
        const T val_min,
        const T val_max,
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
    void step() override
    {
        const T current = *_ptr_input;
        if (current < _val_min)
        {
            *_ptr_output = _val_min;
        }
        else if (current > _val_max)
        {
            *_ptr_output = _val_max;
        }
        else
        {
            *_ptr_output = current;
        }
    }

protected:
    const T* _ptr_input;
    T* _ptr_output;
    const T _val_min;
    const T _val_max;
};

Limiter::Limiter()
{
    in_port_value = nullptr;
    output_port_value = nullptr;
    output_port = std::make_unique<PortValue>(
    PortValue {
        .dtype = DataType::UNKNOWN,
        .ptr = nullptr
    });
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
    return {};
}

size_t Limiter::get_num_inputs() const
{
    return 1;
}

size_t Limiter::get_num_outputs() const
{
    return 1;
}

bool Limiter::update_block()
{
    if (in_port_value == nullptr)
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
    else if (in_port_value->dtype != output_port->dtype)
    {
        output_port->dtype = in_port_value->dtype;

        output_port_value = nullptr;
        output_port->ptr = nullptr;

        switch (output_port->dtype)
        {
        case DataType::DOUBLE:
            output_port_value = std::make_unique<ValueBoxType<double>>(0.0);
            break;
        case DataType::INT32:
            output_port_value = std::make_unique<ValueBoxType<int32_t>>(0);
            break;
        default:
            break;
        }

        if (output_port_value != nullptr)
        {
            output_port->ptr = reinterpret_cast<const void*>(output_port_value.get());
        }

        return true;
    }
    else
    {
        return false;
    }
}

void Limiter::set_input_port(
    const size_t port,
    const PortValue* value)
{
    if (port < get_num_inputs())
    {
        in_port_value = value;
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
    if (in_port_value == nullptr || output_port_value == nullptr)
    {
        throw ModelException("cannot execute with incomplete input parameters");
    }

    switch (in_port_value->dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<LimiterExecutor<double>>(
            reinterpret_cast<const double*>(in_port_value->ptr),
            -10.0,
            10.0,
            reinterpret_cast<double*>(output_port_value->get_ptr_val()));
    default:
        return nullptr;
    }
}
