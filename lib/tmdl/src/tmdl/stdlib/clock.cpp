// SPDX-License-Identifier: GPL-3.0-only

#include "clock.hpp"

class ClockExecutor : public tmdl::BlockExecutionInterface
{
public:
    ClockExecutor(
        double* ptr_output) :
        _ptr_output(ptr_output)
    {
        if (ptr_output == nullptr)
        {
            throw tmdl::ModelException("input pointers cannot be null");
        }
    }

public:
    void step(const tmdl::SimState& s) override
    {
        *_ptr_output = s.time;
    }

protected:
    double* _ptr_output;
};


tmdl::stdlib::Clock::Clock()
{
    output_port_value = std::make_unique<ValueBoxType<double>>(0.0);

    output_port = std::make_unique<PortValue>();
    output_port->dtype = DataType::DOUBLE;
    output_port->ptr = output_port_value->get_ptr_val();
}

std::string tmdl::stdlib::Clock::get_name() const
{
    return "clock";
}

std::string tmdl::stdlib::Clock::get_description() const
{
    return "Clock to provide the current time in seconds";
}

size_t tmdl::stdlib::Clock::get_num_inputs() const
{
    return 0;
}

size_t tmdl::stdlib::Clock::get_num_outputs() const
{
    return 1;
}

bool tmdl::stdlib::Clock::update_block()
{
    return true;
}

void tmdl::stdlib::Clock::set_input_port(
    const size_t,
    const PortValue*)
{
    throw ModelException("no input ports for clock");
}

const tmdl::PortValue* tmdl::stdlib::Clock::get_output_port(const size_t port) const
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

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Clock::get_execution_interface() const
{
    return std::make_shared<ClockExecutor>(reinterpret_cast<double*>(output_port_value->get_ptr_val()));
}
