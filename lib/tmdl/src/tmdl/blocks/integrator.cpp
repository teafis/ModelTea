// SPDX-License-Identifier: GPL-3.0-only

#include "integrator.hpp"

#include <memory>

#include "../model_exception.hpp"

#include <tmdlstd/integrator.hpp>

template <tmdl::DataType DT>
struct IntegratorExecutor : public tmdl::BlockExecutionInterface
{
    static_assert(tmdl::data_type_t<DT>::is_numeric);
    using type_t = tmdl::data_type_t<DT>::type;

    IntegratorExecutor(
        std::shared_ptr<const tmdl::ModelValue> input,
        std::shared_ptr<const tmdl::ModelValue> reset_value,
        std::shared_ptr<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> reset_flag,
        std::shared_ptr<tmdl::ModelValue> output) :
        _input(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(input)),
        _reset_value(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(reset_value)),
        _output(std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(output)),
        _reset_flag(reset_flag)
    {
        if (_input == nullptr || _reset_value == nullptr || _reset_flag == nullptr || _output == nullptr)
        {
            throw tmdl::ModelException("all pointers must be non-null");
        }
    }

    void init(const tmdl::SimState& s) override
    {
        block = std::make_unique<tmdl::stdlib::integrator_block<type_t>>(s.get_dt());

        block->s_in.input_value = &_input->value;
        block->s_in.reset_value = &_reset_value->value;
        block->s_in.reset_flag = &_reset_flag->value;

        block->init();
    }

    void step(const tmdl::SimState&) override
    {
        block->step();
        _output->value = block->s_out.output_value;
    }

    void reset(const tmdl::SimState&) override
    {
        block->reset();
    }

    void close() override
    {
        block = nullptr;
    }

protected:
    std::shared_ptr<const tmdl::ModelValueBox<DT>> _input;
    std::shared_ptr<const tmdl::ModelValueBox<DT>> _reset_value;
    std::shared_ptr<tmdl::ModelValueBox<DT>> _output;

    std::shared_ptr<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> _reset_flag;

    std::unique_ptr<tmdl::stdlib::integrator_block<type_t>> block;
};

tmdl::blocks::Integrator::Integrator()
{
    input_type = DataType::UNKNOWN;
    input_reset_flag_type = DataType::UNKNOWN;
    input_reset_value_type = DataType::UNKNOWN;
}

std::string tmdl::blocks::Integrator::get_name() const
{
    return "integrator";
}

std::string tmdl::blocks::Integrator::get_description() const
{
    return "integrates the provided values, or resets to the provided value";
}

size_t tmdl::blocks::Integrator::get_num_inputs() const
{
    return 3;
}

size_t tmdl::blocks::Integrator::get_num_outputs() const
{
    return 1;
}

bool tmdl::blocks::Integrator::update_block()
{
    if (output_port != input_type)
    {
        output_port = input_type;
        return true;
    }

    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::Integrator::has_error() const
{
    if (input_type != DataType::DOUBLE && input_type != DataType::SINGLE)
    {
        return make_error("integrator only works with floating point types");
    }
    if (input_type != output_port)
    {
        return make_error("input port doesn't match output port type");
    }
    else if (input_type != input_reset_value_type)
    {
        return make_error("input port value and reset value types don't match");
    }
    else if (input_reset_flag_type != DataType::BOOLEAN)
    {
        return make_error("reset flag must be a boolean type");
    }

    return nullptr;
}

void tmdl::blocks::Integrator::set_input_type(
    const size_t port,
    const DataType type)
{
    switch (port)
    {
    case 0:
        input_type = type;
        break;
    case 1:
        input_reset_flag_type = type;
        break;
    case 2:
        input_reset_value_type = type;
        break;
    default:
        throw ModelException("input port out of range");
    }
}

tmdl::DataType tmdl::blocks::Integrator::get_output_type(const size_t port) const
{
    if (port == 0)
    {
        return output_port;
    }
    else
    {
        throw ModelException("output port out of range");
    }
}

bool tmdl::blocks::Integrator::outputs_are_delayed() const
{
    return true;
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::blocks::Integrator::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    const auto err = has_error();
    if (err != nullptr)
    {
        throw ModelException("cannot creator interface with an error");
    }

    const auto in_value = manager.get_ptr(*connections.get_connection_to(get_id(), 0));
    const auto in_reset_flag = std::dynamic_pointer_cast<const ModelValueBox<DataType::BOOLEAN>>(manager.get_ptr(*connections.get_connection_to(get_id(), 1)));
    const auto in_reset_value = manager.get_ptr(*connections.get_connection_to(get_id(), 2));

    const auto out_value = manager.get_ptr(VariableIdentifier {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (input_type)
    {
    case DataType::DOUBLE:
        return std::make_shared<IntegratorExecutor<DataType::DOUBLE>>(in_value, in_reset_value, in_reset_flag, out_value);
    case DataType::SINGLE:
        return std::make_shared<IntegratorExecutor<DataType::SINGLE>>(in_value, in_reset_value, in_reset_flag, out_value);
    default:
        throw ModelException("unable to create pointer value");
    }
}
