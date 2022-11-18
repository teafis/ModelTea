// SPDX-License-Identifier: GPL-3.0-only

#include "trig.hpp"

tmdl::stdlib::TrigFunction::TrigFunction()
{
    input_value = nullptr;
    output_port = std::make_unique<PortValue>();
}

size_t tmdl::stdlib::TrigFunction::get_num_inputs() const
{
    return 1;
}

size_t tmdl::stdlib::TrigFunction::get_num_outputs() const
{
    return 1;
}

bool tmdl::stdlib::TrigFunction::update_block()
{
    if (input_value == nullptr)
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
    else if (input_value->dtype != output_port->dtype)
    {
        switch (input_value->dtype)
        {
        case DataType::DOUBLE:
            output_port_value = std::make_unique<ValueBoxType<double>>(0.0);
            break;
        case DataType::SINGLE:
            output_port_value = std::make_unique<ValueBoxType<float>>(0.0);
            break;
        default:
            output_port_value = nullptr;
        }

        if (output_port_value)
        {
            output_port->dtype = input_value->dtype;
            output_port->ptr = output_port_value->get_ptr_val();
        }
        else
        {
            output_port->dtype = DataType::UNKNOWN;
            output_port->ptr = nullptr;
        }

        return true;
    }
    else
    {
        return false;
    }
}

std::unique_ptr<const tmdl::BlockError> tmdl::stdlib::TrigFunction::has_error() const
{
    if (input_value == nullptr)
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
            .message = "output port value unable to be determined"
        });
    }

    return nullptr;
}

void tmdl::stdlib::TrigFunction::set_input_port(
    const size_t port,
    const PortValue* value)
{
    if (port == 0)
    {
        input_value = value;
    }
    else
    {
        throw ModelException("invalid input port provided");
    }
}

const tmdl::PortValue* tmdl::stdlib::TrigFunction::get_output_port(const size_t port) const
{
    if (port == 0)
    {
        return output_port.get();
    }
    else
    {
        throw ModelException("invalid output port provided");
    }
}

template <typename T, T (FNC)( T)>
class TrigExecutor : public tmdl::BlockExecutionInterface
{
public:
    TrigExecutor(
        const T* ptr_input,
        T* ptr_output) :
        _ptr_input(ptr_input),
        _ptr_output(ptr_output)
    {
        if (ptr_input == nullptr || ptr_output == nullptr)
        {
            throw tmdl::ModelException("input pointers cannot be null");
        }
    }

public:
    void step(const tmdl::SimState&) override
    {
        *_ptr_output = FNC(*_ptr_input);
    }

protected:
    const T* _ptr_input;
    T* _ptr_output;
};

std::string tmdl::stdlib::TrigSin::get_name() const
{
    return "sin";
}

std::string tmdl::stdlib::TrigSin::get_description() const
{
    return "computes the sin of the input parameter";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::TrigSin::get_execution_interface() const
{
    if (input_value == nullptr || output_port_value == nullptr)
    {
        throw ModelException("cannot execute with incomplete input parameters");
    }

    switch (input_value->dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<TrigExecutor<double, std::sin>>(
            reinterpret_cast<const double*>(input_value->ptr),
            reinterpret_cast<double*>(output_port_value->get_ptr_val()));
    case DataType::SINGLE:
        return std::make_shared<TrigExecutor<float, std::sin>>(
            reinterpret_cast<const float*>(input_value->ptr),
            reinterpret_cast<float*>(output_port_value->get_ptr_val()));
    default:
        throw ModelException("unable to generate limitor executor");
    }
}

std::string tmdl::stdlib::TrigCos::get_name() const
{
    return "cos";
}

std::string tmdl::stdlib::TrigCos::get_description() const
{
    return "computes the cos of the input parameter";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::TrigCos::get_execution_interface() const
{
    if (input_value == nullptr || output_port_value == nullptr)
    {
        throw ModelException("cannot execute with incomplete input parameters");
    }

    switch (input_value->dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<TrigExecutor<double, std::cos>>(
            reinterpret_cast<const double*>(input_value->ptr),
            reinterpret_cast<double*>(output_port_value->get_ptr_val()));
    case DataType::SINGLE:
        return std::make_shared<TrigExecutor<float, std::cos>>(
            reinterpret_cast<const float*>(input_value->ptr),
            reinterpret_cast<float*>(output_port_value->get_ptr_val()));
    default:
        throw ModelException("unable to generate limitor executor");
    }
}
