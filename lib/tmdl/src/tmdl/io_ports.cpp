// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/io_ports.hpp>

using namespace tmdl;

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
    const PortValue)
{
    throw ModelException("cannot set input port value");
}

PortValue InputPort::get_output_port(const size_t port) const
{
    if (port == 0)
    {
        return _port;
    }
    else
    {
        throw ModelException("cannot provide output for provided port number");
    }
}

bool InputPort::update_block()
{
    return false;
}

/*
BlockExecutionInterface* InputPort::get_executor() const
{
    return _executor.get();
}
*/

void InputPort::set_input_value(const PortValue value)
{
    _port = value;
}

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
    const PortValue value)
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

PortValue OutputPort::get_output_port(const size_t /* port */) const
{
    throw ModelException("cannot get input port value");
}

bool OutputPort::update_block()
{
    return false;
}

/*
BlockExecutionInterface* OutputPort::get_executor() const
{
    return _executor.get();
}
*/

PortValue OutputPort::get_output_value() const
{
    return _port;
}
