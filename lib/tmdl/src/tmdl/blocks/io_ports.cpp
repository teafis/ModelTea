// SPDX-License-Identifier: GPL-3.0-only

#include "io_ports.hpp"

#include "../model_exception.hpp"


using namespace tmdl;

/* ========== INPUT PORT ========== */

std::string InputPort::get_name() const
{
    return "IN";
}

std::string InputPort::get_description() const
{
    return "Input Port";
}

std::vector<Parameter*> InputPort::get_parameters() const
{
    return {};
}

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

std::unique_ptr<const BlockError> InputPort::has_error() const
{
    if (_port.dtype == DataType::UNKNOWN)
    {
        return std::make_unique<BlockError>(
        BlockError
        {
            .id = get_id(),
            .message = "input port has unknown type"
        });
    }

    return nullptr;
}

bool InputPort::update_block()
{
    return false;
}

std::shared_ptr<BlockExecutionInterface> InputPort::get_execution_interface(
    const ConnectionManager&,
    const VariableManager&) const
{
    return std::make_shared<BlockExecutionInterface>();
}

void InputPort::set_input_value(const PortValue value)
{
    _port = value;
}

/* ========== OUTPUT PORT ========== */

std::string OutputPort::get_name() const
{
    return "OUT";
}

std::string OutputPort::get_description() const
{
    return "Output Port";
}

std::vector<Parameter*> OutputPort::get_parameters() const
{
    return {};
}

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

std::unique_ptr<const BlockError> OutputPort::has_error() const
{
    if (_port.dtype == DataType::UNKNOWN)
    {
        return std::make_unique<BlockError>(
        BlockError
        {
            .id = get_id(),
            .message = "output port has unknown type"
        });
    }

    return nullptr;
}

bool OutputPort::update_block()
{
    return false;
}

std::shared_ptr<BlockExecutionInterface> OutputPort::get_execution_interface(
    const ConnectionManager&,
    const VariableManager&) const
{
    return std::make_shared<BlockExecutionInterface>();
}

PortValue OutputPort::get_output_value() const
{
    return _port;
}
