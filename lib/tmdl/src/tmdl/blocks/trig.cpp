// SPDX-License-Identifier: GPL-3.0-only

#include "trig.hpp"

#include "../model_exception.hpp"


tmdl::stdlib::TrigFunction::TrigFunction()
{
    input_type = DataType::UNKNOWN;
}

size_t tmdl::stdlib::TrigFunction::get_num_inputs() const
{
    return 1;
}

size_t tmdl::stdlib::TrigFunction::get_num_outputs() const
{
    return 1;
}

bool tmdl::stdlib::TrigFunction::update_block()
{
    if (input_type != output_port.dtype)
    {
        output_port.dtype = input_type;
        return true;
    }
    else
    {
        return false;
    }
}

std::unique_ptr<const tmdl::BlockError> tmdl::stdlib::TrigFunction::has_error() const
{
    if (input_type != output_port.dtype)
    {
        return make_error("mismatch in input and output types");
    }
    else if (input_type != DataType::DOUBLE && input_type != DataType::SINGLE)
    {
        return make_error("invalid input port type provided");
    }

    return nullptr;
}

void tmdl::stdlib::TrigFunction::set_input_port(
    const size_t port,
    const DataType type)
{
    if (port == 0)
    {
        input_type = type;
    }
    else
    {
        throw ModelException("invalid input port provided");
    }
}

tmdl::PortValue tmdl::stdlib::TrigFunction::get_output_port(const size_t port) const
{
    if (port == 0)
    {
        return output_port;
    }
    else
    {
        throw ModelException("invalid output port provided");
    }
}

template <typename T, T (FNC)(T)>
class TrigExecutor : public tmdl::BlockExecutionInterface
{
public:
    TrigExecutor(
        std::shared_ptr<const tmdl::ValueBox> ptr_input,
        std::shared_ptr<tmdl::ValueBox> ptr_output) :
        _ptr_input(std::dynamic_pointer_cast<const tmdl::ValueBoxType<T>>(ptr_input)),
        _ptr_output(std::dynamic_pointer_cast<tmdl::ValueBoxType<T>>(ptr_output))
    {
        if (_ptr_input == nullptr || _ptr_output == nullptr)
        {
            throw tmdl::ModelException("input pointers cannot be null");
        }
    }

public:
    void step(const tmdl::SimState&) override
    {
        _ptr_output->value = FNC(_ptr_input->value);
    }

protected:
    const std::shared_ptr<const tmdl::ValueBoxType<T>> _ptr_input;
    std::shared_ptr<tmdl::ValueBoxType<T>> _ptr_output;
};

std::string tmdl::stdlib::TrigSin::get_name() const
{
    return "sin";
}

std::string tmdl::stdlib::TrigSin::get_description() const
{
    return "computes the sin of the input parameter";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::TrigSin::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot execute with incomplete input parameters");
    }

    const auto inputValue = manager.get_ptr(connections.get_connection_to(get_id(), 0));
    const auto outputValue = manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (input_type)
    {
    case DataType::DOUBLE:
        return std::make_shared<TrigExecutor<double, std::sin>>(
            inputValue,
            outputValue);
    case DataType::SINGLE:
        return std::make_shared<TrigExecutor<float, std::sin>>(
            inputValue,
            outputValue);
    default:
        throw ModelException("unable to generate limitor executor");
    }
}

std::string tmdl::stdlib::TrigCos::get_name() const
{
    return "cos";
}

std::string tmdl::stdlib::TrigCos::get_description() const
{
    return "computes the cos of the input parameter";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::TrigCos::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot execute with incomplete input parameters");
    }

    const auto inputValue = manager.get_ptr(connections.get_connection_to(get_id(), 0));
    const auto outputValue = manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (input_type)
    {
    case DataType::DOUBLE:
        return std::make_shared<TrigExecutor<double, std::cos>>(
            inputValue,
            outputValue);
    case DataType::SINGLE:
        return std::make_shared<TrigExecutor<float, std::cos>>(
            inputValue,
            outputValue);
    default:
        throw ModelException("unable to generate limitor executor");
    }
}
