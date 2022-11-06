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

std::shared_ptr<BlockExecutionInterface> InputPort::get_execution_interface() const
{
    return std::make_shared<Executor>(this);
}

InputPort::Executor::Executor(const InputPort* parent) : BlockExecutionInterface(parent)
{
    // Empty Constructor
}

void InputPort::Executor::set_value(std::shared_ptr<const Value> value)
{
    _value = value;
}

void InputPort::Executor::set_input_value(
    const size_t,
    std::shared_ptr<const Value>)
{
    throw ModelException("unable to set input value");
}

std::shared_ptr<const Value> InputPort::Executor::get_output_value(const size_t port) const
{
    if (port == 0)
    {
        return _value;
    }
    else
    {
        return nullptr;
    }
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

std::shared_ptr<BlockExecutionInterface> OutputPort::get_execution_interface() const
{
    return std::make_shared<Executor>(this);
}

OutputPort::Executor::Executor(const OutputPort* parent) : BlockExecutionInterface(parent)
{
    // Empty Constructor
}

void OutputPort::Executor::set_input_value(
    const size_t port,
    const std::shared_ptr<const Value> value)
{
    if (port == 0)
    {
        _value = value;
    }
    else
    {
        throw ModelException("unable to set input port value");
    }
}

std::shared_ptr<const Value> OutputPort::Executor::get_output_value(const size_t) const
{
    throw ModelException("No output value for OutputPort");
}

std::shared_ptr<const Value> OutputPort::Executor::get_value() const
{
    return _value;
}
