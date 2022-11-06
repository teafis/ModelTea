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

std::unique_ptr<BlockExecutionInterface> InputPort::get_execution_interface() const
{
    return std::make_unique<InputPortExecutor>(this);
}

InputPort::InputPortExecutor::InputPortExecutor(const InputPort* parent) : BlockExecutionInterface(parent)
{
    // Empty Constructor
}

void InputPort::InputPortExecutor::set_value(std::shared_ptr<const Value> value)
{
    _value = value;
}

void InputPort::InputPortExecutor::set_input_value(
    const size_t,
    std::shared_ptr<const Value>)
{
    throw ModelException("unable to set input value");
}

std::shared_ptr<const Value> InputPort::InputPortExecutor::get_output_value(const size_t port) const
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

std::unique_ptr<BlockExecutionInterface> OutputPort::get_execution_interface() const
{
    return std::make_unique<OutputPortExecutor>(this);
}

OutputPort::OutputPortExecutor::OutputPortExecutor(const OutputPort* parent) : BlockExecutionInterface(parent)
{
    // Empty Constructor
}

void OutputPort::OutputPortExecutor::set_input_value(
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

std::shared_ptr<const Value> OutputPort::OutputPortExecutor::get_output_value(const size_t) const
{
    throw ModelException("No output value for OutputPort");
}

std::shared_ptr<const Value> OutputPort::OutputPortExecutor::get_value() const
{
    return _value;
}
