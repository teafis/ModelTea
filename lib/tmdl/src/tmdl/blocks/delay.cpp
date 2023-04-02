// SPDX-License-Identifier: GPL-3.0-only

#include "delay.hpp"
#include "../model_exception.hpp"

#include <fmt/format.h>

#include <tmdlstd/delay.hpp>
#include <tmdlstd/util.hpp>

#include <memory>


template <tmdl::DataType DT>
struct DelayComponent : public tmdl::codegen::CodeComponent
{
    virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
    {
        return tmdl::codegen::InterfaceDefinition("s_out", {"input_value", "reset_flag", "reset_value"});
    }

    virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
    {
        return tmdl::codegen::InterfaceDefinition("s_out", {"output_value"});
    }

    virtual std::string get_include_file_name() const override
    {
        return "tmdlstd/delay.hpp";
    }

    virtual std::string get_name_base() const override
    {
        return "delay_block";
    }

    virtual std::string get_type_name() const override
    {
        return fmt::format("tmdlstd::delay_block<{}>", tmdl::data_type_to_string(DT));
    }

    virtual std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override
    {
        switch (ft)
        {
        case tmdl::codegen::BlockFunction::INIT:
            return "init";
        case tmdl::codegen::BlockFunction::RESET:
            return "reset";
        case tmdl::codegen::BlockFunction::STEP:
            return "step";
        default:
            return {};
        }
    }

protected:
    virtual std::vector<std::string> write_cpp_code(tmdl::codegen::CodeSection section) const override
    {
        (void)section;
        return {};
    }
};

template <tmdl::DataType DT>
class DelayExecutor : public tmdl::BlockExecutionInterface
{
public:
    using type_t = typename tmdl::data_type_t<DT>::type;

    DelayExecutor(
        const std::shared_ptr<const tmdl::ModelValue> input,
        const std::shared_ptr<tmdl::ModelValue> output,
        const std::shared_ptr<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> reset_flag,
        const std::shared_ptr<const tmdl::ModelValue> reset_value) :
        _input(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(input)),
        _output(std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(output)),
        _reset_flag(reset_flag),
        _reset_value(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(reset_value))
    {
        if (_input == nullptr || _output == nullptr || _reset_flag == nullptr || _reset_value == nullptr)
        {
            throw tmdl::ModelException("input values must be non-null");
        }

        block.s_in.input_value = &_input->value;
        block.s_in.reset_value = &_reset_value->value;
        block.s_in.reset_flag = &_reset_flag->value;
    }

    void init(const tmdl::SimState&) override
    {
        block.init();
    }

    void step(const tmdl::SimState&) override
    {
        block.step();
        _output->value = block.s_out.output_value;
    }

    void reset(const tmdl::SimState&) override
    {
        block.reset();
    }

protected:
    tmdl::stdlib::delay_block<type_t> block;

    std::shared_ptr<const tmdl::ModelValueBox<DT>> _input;
    std::shared_ptr<tmdl::ModelValueBox<DT>> _output;
    std::shared_ptr<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> _reset_flag;
    std::shared_ptr<const tmdl::ModelValueBox<DT>> _reset_value;
};

tmdl::blocks::Delay::Delay()
{
    input_type = DataType::UNKNOWN;
    input_reset_flag = DataType::UNKNOWN;
    input_reset_value = DataType::UNKNOWN;
    output_port = DataType::UNKNOWN;
}

std::string tmdl::blocks::Delay::get_name() const
{
    return "delay";
}

std::string tmdl::blocks::Delay::get_description() const
{
    return "delays the provided input value by one cycle";
}

size_t tmdl::blocks::Delay::get_num_inputs() const
{
    return 3;
}

size_t tmdl::blocks::Delay::get_num_outputs() const
{
    return 1;
}

bool tmdl::blocks::Delay::update_block()
{
    if (input_type != output_port)
    {
        output_port = input_type;
        return true;
    }

    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::Delay::has_error() const
{
    if (input_type != output_port)
    {
        return make_error("input type does not equal output type");
    }
    else if (input_reset_flag != DataType::BOOLEAN)
    {
        return make_error("reset flag must be boolean");
    }
    else if (input_type != input_reset_value)
    {
        return make_error("input type must equal the reset value type");
    }

    return nullptr;
}

void tmdl::blocks::Delay::set_input_type(
    const size_t port,
    const DataType type)
{
    switch (port)
    {
    case 0:
        input_type = type;
        break;
    case 1:
        input_reset_flag = type;
        break;
    case 2:
        input_reset_value = type;
        break;
    default:
        throw ModelException("invalid input port provided");
    }
}

tmdl::DataType tmdl::blocks::Delay::get_output_type(const size_t port) const
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

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::blocks::Delay::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot build executor with error");
    }

    auto input_value = manager.get_ptr(*connections.get_connection_to(get_id(), 0));

    auto input_value_reset_flag = std::dynamic_pointer_cast<const ModelValueBox<DataType::BOOLEAN>>(manager.get_ptr(*connections.get_connection_to(get_id(), 1)));
    auto input_value_reset_value = manager.get_ptr(*connections.get_connection_to(get_id(), 2));

    auto output_value = manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (output_port)
    {
    case DataType::DOUBLE:
        return std::make_shared<DelayExecutor<DataType::DOUBLE>>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    case DataType::SINGLE:
        return std::make_shared<DelayExecutor<DataType::SINGLE>>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    case DataType::BOOLEAN:
        return std::make_shared<DelayExecutor<DataType::BOOLEAN>>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    case DataType::INT32:
        return std::make_shared<DelayExecutor<DataType::INT32>>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    case DataType::UINT32:
        return std::make_shared<DelayExecutor<DataType::UINT32>>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    default:
        throw ModelException("unknown data type provided");
    }
}
