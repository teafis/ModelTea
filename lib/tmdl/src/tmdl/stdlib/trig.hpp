// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_TRIG_HPP
#define TF_MODEL_STDLIB_TRIG_HPP

#include <tmdl/block.hpp>

#include <cmath>

namespace tmdl::stdlib
{

class TrigFunction : public Block
{
public:
    size_t get_num_inputs() const override
    {
        return 1;
    }

    size_t get_num_outputs() const override
    {
        return 1;
    }

    bool update_block() override
    {
        if (input_value == nullptr)
        {
            output_port_value = nullptr;
            return false;
        }
        else if (output_port == nullptr || input_value->dtype != output_port->dtype)
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
                return true;
            }
            else
            {
                output_port->dtype = DataType::UNKNOWN;
                output_port->ptr = nullptr;
                return false;
            }
        }
        else
        {
            return true;
        }
    }

    void set_input_port(
        const size_t port,
        const PortValue* value) override
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

    const PortValue* get_output_port(const size_t port) const override
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

protected:
    const PortValue* input_value;

    std::unique_ptr<PortValue> output_port;
    std::unique_ptr<ValueBox> output_port_value;
};

template <typename T, T (FNC)( T)>
class TrigExecutor : public BlockExecutionInterface
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
            throw ModelException("input pointers cannot be null");
        }
    }

public:
    void step(const SimState&) override
    {
        *_ptr_output = FNC(*_ptr_input);
    }

protected:
    const T* _ptr_input;
    T* _ptr_output;
};

class TrigSin : public TrigFunction
{
public:
    std::string get_name() const override
    {
        return "sin";
    }

    std::string get_description() const override
    {
        return "computes the sin of the input parameter";
    }

    std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override
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
};

class TrigCos : public TrigFunction
{
public:
    std::string get_name() const override
    {
        return "cos";
    }

    std::string get_description() const override
    {
        return "computes the cos of the input parameter";
    }

    std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override
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
};

}

#endif // TF_MODEL_STDLIB_TRIG_HPP
