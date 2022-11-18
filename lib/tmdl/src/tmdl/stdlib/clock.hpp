// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_CLOCK_HPP
#define TF_MODEL_STDLIB_CLOCK_HPP

#include <tmdl/block.hpp>

namespace tmdl::stdlib
{

class ClockExecutor : public BlockExecutionInterface
{
public:
    ClockExecutor(
        double* ptr_output) :
        _ptr_output(ptr_output)
    {
        if (ptr_output == nullptr)
        {
            throw ModelException("input pointers cannot be null");
        }
    }

public:
    void step(const SimState& s) override
    {
        *_ptr_output = s.time;
    }

protected:
    double* _ptr_output;
};

class Clock : public Block
{
public:
    Clock()
    {
        output_port_value = std::make_unique<ValueBoxType<double>>(0.0);

        output_port = std::make_unique<PortValue>();
        output_port->dtype = DataType::DOUBLE;
        output_port->ptr = output_port_value->get_ptr_val();
    }

    std::string get_name() const override
    {
        return "clock";
    }

    std::string get_description() const override
    {
        return "Clock to provide the current time in seconds";
    }

    size_t get_num_inputs() const override
    {
        return 0;
    }

    size_t get_num_outputs() const override
    {
        return 1;
    }

    bool update_block() override
    {
        return true;
    }

    void set_input_port(
        const size_t,
        const PortValue*) override
    {
        throw ModelException("no input ports for clock");
    }

    const PortValue* get_output_port(const size_t port) const override
    {
        if (port == 0)
        {
            return output_port.get();
        }
        else
        {
            throw ModelException("invalid output port requested");
        }
    }

    std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override
    {
        return std::make_shared<ClockExecutor>(reinterpret_cast<double*>(output_port_value->get_ptr_val()));
    }

protected:
    std::unique_ptr<PortValue> output_port;
    std::unique_ptr<ValueBox> output_port_value;
};

}

#endif // TF_MODEL_STDLIB_CLOCK_HPP
