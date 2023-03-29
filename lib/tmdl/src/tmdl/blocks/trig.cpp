// SPDX-License-Identifier: GPL-3.0-only

#include "trig.hpp"

#include "../model_exception.hpp"

#include <tmdlstd/trig.hpp>


tmdl::blocks::TrigFunction::TrigFunction()
{
    input_type = DataType::UNKNOWN;
}

size_t tmdl::blocks::TrigFunction::get_num_inputs() const
{
    return 1;
}

size_t tmdl::blocks::TrigFunction::get_num_outputs() const
{
    return 1;
}

bool tmdl::blocks::TrigFunction::update_block()
{
    if (input_type != output_port)
    {
        output_port = input_type;
        return true;
    }
    else
    {
        return false;
    }
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::TrigFunction::has_error() const
{
    if (input_type != output_port)
    {
        return make_error("mismatch in input and output types");
    }
    else if (input_type != DataType::DOUBLE && input_type != DataType::SINGLE)
    {
        return make_error("invalid input port type provided");
    }

    return nullptr;
}

void tmdl::blocks::TrigFunction::set_input_type(
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

tmdl::DataType tmdl::blocks::TrigFunction::get_output_type(const size_t port) const
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

template <tmdl::DataType DT, tmdl::stdlib::TrigFunction FCN>
class TrigExecutor : public tmdl::BlockExecutionInterface
{
public:
    using val_t = typename tmdl::data_type_t<DT>::type;

public:
    TrigExecutor(
        std::shared_ptr<const tmdl::ModelValue> ptr_input,
        std::shared_ptr<tmdl::ModelValue> ptr_output) :
        _ptr_input(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(ptr_input)),
        _ptr_output(std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(ptr_output))
    {
        if (_ptr_input == nullptr || _ptr_output == nullptr)
        {
            throw tmdl::ModelException("input pointers cannot be null");
        }

        block.s_in.value = &_ptr_input->value;
    }

public:
    void step(const tmdl::SimState&) override
    {
        block.step();
        _ptr_output->value = block.s_out.value;
    }

protected:
    const std::shared_ptr<const tmdl::ModelValueBox<DT>> _ptr_input;
    const std::shared_ptr<tmdl::ModelValueBox<DT>> _ptr_output;
    tmdl::stdlib::trig_block<val_t, FCN> block;
};

template <tmdl::stdlib::TrigFunction FCN>
static std::shared_ptr<tmdl::BlockExecutionInterface> generate_exec_interface(
    const tmdl::blocks::TrigFunction* model,
    const tmdl::ConnectionManager& connections,
    const tmdl::VariableManager& manager)
{
    if (model->has_error() != nullptr)
    {
        throw tmdl::ModelException("cannot execute with incomplete input parameters");
    }

    const auto inputValue = manager.get_ptr(*connections.get_connection_to(model->get_id(), 0));
    const auto outputValue = manager.get_ptr(tmdl::VariableIdentifier
    {
        .block_id = model->get_id(),
        .output_port_num = 0
    });

    switch (inputValue->get_data_type())
    {
    case tmdl::DataType::DOUBLE:
        return std::make_shared<TrigExecutor<tmdl::DataType::DOUBLE, FCN>>(
            inputValue,
            outputValue);
    case tmdl::DataType::SINGLE:
        return std::make_shared<TrigExecutor<tmdl::DataType::SINGLE, FCN>>(
            inputValue,
            outputValue);
    default:
        throw tmdl::ModelException("unable to generate limitor executor");
    }
}

std::string tmdl::blocks::TrigSin::get_name() const
{
    return "sin";
}

std::string tmdl::blocks::TrigSin::get_description() const
{
    return "computes the sin of the input parameter";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::blocks::TrigSin::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    return generate_exec_interface<tmdl::stdlib::TrigFunction::SIN>(this, connections, manager);
}

std::string tmdl::blocks::TrigCos::get_name() const
{
    return "cos";
}

std::string tmdl::blocks::TrigCos::get_description() const
{
    return "computes the cos of the input parameter";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::blocks::TrigCos::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    return generate_exec_interface<tmdl::stdlib::TrigFunction::COS>(this, connections, manager);
}
