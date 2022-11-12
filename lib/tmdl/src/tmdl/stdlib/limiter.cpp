#include "limiter.hpp"

using namespace tmdl;
using namespace tmdl::stdlib;


template <typename T>
class LimiterExecutor : public BlockExecutionInterface
{
public:
    LimiterExecutor(
        const PortValue* input_port,
        const T val_min,
        const T val_max) :
        _input_port(input_port),
        _val_min(val_min),
        _val_max(val_max)
    {
        if (input_port == nullptr)
        {
            throw ModelException("input pointers cannot be null");
        }
        else if (val_max < val_min)
        {
            throw ModelException("maximum value must be above the minimum value");
        }
    }

    const T* get_output_value()
    {
        return &_val_output;
    }

public:
    void step() override
    {
        const T current = *reinterpret_cast<const T*>(_input_port->ptr);
        if (current < _val_min)
        {
            _val_output = _val_min;
        }
        else if (current > _val_max)
        {
            _val_output = _val_max;
        }
        else
        {
            _val_output = current;
        }
    }

protected:
    const PortValue* _input_port;
    T _val_output;
    const T _val_min;
    const T _val_max;
};

Limiter::Limiter()
{
    in_port_value = std::make_shared<const PortValue*>(nullptr);
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
    if (*in_port_value == nullptr)
    {
        executor = nullptr;

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
    else if ((*in_port_value)->dtype != output_port->dtype)
    {
        output_port->dtype = (*in_port_value)->dtype;

        switch (output_port->dtype)
        {
        case DataType::DOUBLE:
        {
            auto ptr = std::make_shared<LimiterExecutor<double>>(*in_port_value, -10.0, 10.0);
            output_port->ptr = reinterpret_cast<const void*>(ptr->get_output_value());
            executor = ptr;
            break;
        }
        case DataType::INT32:
        {
            auto ptr = std::make_shared<LimiterExecutor<int32_t>>(*in_port_value, -10, 10);
            output_port->ptr = reinterpret_cast<const void*>(ptr->get_output_value());
            executor = ptr;
            break;
        }
        default:
            output_port->ptr = nullptr;
            executor = nullptr;
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
        *in_port_value = value;
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

BlockExecutionInterface* Limiter::get_execution_interface() const
{
    return executor.get();
}
