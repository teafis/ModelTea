// SPDX-License-Identifier: GPL-3.0-only

#include "io_ports.hpp"

#include "../model_exception.hpp"


using namespace tmdl;

/* ========== INPUT PORT ========== */

InputPort::InputPort()
{
    ParameterValue pv = ParameterValue
    {
        .dtype = ParameterValue::Type::DATA_TYPE,
        .value = ParameterValue::Value
        {
            .dtype = DataType::UNKNOWN
        }
    };

    dataTypeParameter = std::make_shared<Parameter>("data_type", "parameter data type", pv);
}

std::string InputPort::get_name() const
{
    return "IN";
}

std::string InputPort::get_description() const
{
    return "Input Port";
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
    const DataType)
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
        return make_error("input port has unknown type");
    }

    return nullptr;
}

std::vector<std::shared_ptr<tmdl::Parameter>> InputPort::get_parameters() const
{
    return {
        dataTypeParameter
    };
}

bool InputPort::update_block()
{
    const auto param_dt = dataTypeParameter->get_value().value.dtype;
    if (param_dt != _port.dtype)
    {
        _port.dtype = param_dt;
        return true;
    }

    return false;
}

std::shared_ptr<BlockExecutionInterface> InputPort::get_execution_interface(
    const ConnectionManager&,
    const VariableManager&) const
{
    return std::make_shared<BlockExecutionInterface>();
}

void InputPort::set_input_value(const DataType type)
{
    _port.dtype = type;
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
    const DataType type)
{
    if (port == 0)
    {
        _port.dtype = type;
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
        return make_error("output port has unknown type");
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
