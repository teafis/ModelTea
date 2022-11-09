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

void InputPort::Executor::set_value(const void* value)
{
    _value = value;
}

void InputPort::Executor::set_input_value(
    const size_t,
    const void*)
{
    throw ModelException("unable to set input value");
}

const void* InputPort::Executor::get_output_value(const size_t port) const
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

template <typename T>
OutputPort::Executor::Executor(const OutputPort* parent) : BlockExecutionInterface(parent)
{
    // Empty Constructor
}

template <typename T>
void OutputPort::Executor<T>::set_input_value(
    const size_t port,
    const void* value)
{
    if (port == 0)
    {
        _port = value;
    }
    else
    {
        throw ModelException("unable to set input port value");
    }
}

template <typename T>
const void* OutputPort::Executor<T>::get_output_value(const size_t) const
{
    throw ModelException("No output value for OutputPort");
}

template <typename T>
const T OutputPort::Executor<T>::get_value() const
{
    return *reinterpret_cast<const T*>(_port->ptr);
}
