// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/io_ports.hpp>

using namespace tmdl;

class PassthroughExecutor : public BlockExecutionInterface
{
public:
    PassthroughExecutor(
        const PortValue* input_port,
        PortValue* output_port) :
        _input(input_port),
        _output(output_port)
    {
        if (input_port == nullptr || output_port == nullptr || *input_port != *output_port)
        {
            throw ModelException("Invalid input/output ports provided");
        }
    }

    void step() override
    {
        if (_input->dtype != _output->dtype)
        {
            throw ModelException("passthrough datatype change at inopportune moment");
        }
    }

protected:
    const PortValue* _input;
    PortValue* _output;
};

/* ========== INPUT PORT ========== */

size_t InputPort::get_num_inputs() const
{
    return 0;
}

size_t InputPort::get_num_outputs() const
{
    return 1;
}

void InputPort::set_input_port(
    const size_t,
    const PortValue*)
{
    throw ModelException("cannot set input port value");
}

const PortValue* InputPort::get_output_port(const size_t port) const
{
    if (port == 0)
    {
        return &_port;
    }
    else
    {
        throw ModelException("cannot provide output for provided port number");
    }
}

void InputPort::set_input_value(const PortValue* value)
{
    if (value == nullptr)
    {
        _port.dtype = DataType::UNKNOWN;
        _port.ptr = nullptr;
    }
    else
    {
        _port.dtype = value->dtype;
        _port.ptr = value->ptr;
    }
}

/*
BlockExecutionInterface* InputPort::get_executor() const
{
    return _executor.get();
}
*/

/* ========== OUTPUT PORT ========== */

size_t OutputPort::get_num_inputs() const
{
    return 1;
}

size_t OutputPort::get_num_outputs() const
{
    return 0;
}

void OutputPort::set_input_port(
    const size_t port,
    const PortValue* value)
{
    if (port == 0)
    {
        _port = value;
    }
    else
    {
        throw ModelException("cannot set input for provided port number");
    }
}

const PortValue* OutputPort::get_output_port(const size_t /* port */) const
{
    throw ModelException("cannot get input port value");
}

/*
BlockExecutionInterface* OutputPort::get_executor() const
{
    return _executor.get();
}
*/

const PortValue* OutputPort::get_output_value() const
{
    return _port;
}
