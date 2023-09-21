// SPDX-License-Identifier: GPL-3.0-only

#include "derivative.hpp"

#include "../model_exception.hpp"

#include <tmdlstd/tmdlstd.hpp>

#include <fmt/format.h>

template <tmdl::DataType DT>
class CompiledDerivative : public tmdl::CompiledBlockInterface
{
public:
    CompiledDerivative(const size_t id, const tmdl::BlockInterface::ModelInfo& s) : _id{ id }, _state(s)
    {
        static_assert(tmdl::data_type_t<DT>::is_numeric);
        static_assert(tmdl::data_type_t<DT>::is_modelable);
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(
        const tmdl::ConnectionManager& connections,
        const tmdl::VariableManager& manager) const override
    {
        auto in_value = manager.get_ptr(*connections.get_connection_to(_id, 0));
        auto in_reset_flag = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>>(manager.get_ptr(*connections.get_connection_to(_id, 1)));

        auto out_value = manager.get_ptr(tmdl::VariableIdentifier {
            .block_id = _id,
            .output_port_num = 0
        });

        return std::make_shared<DerivativeExecutor>(in_value, out_value, in_reset_flag, _state);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override
    {
        return std::make_unique<DerivativeComponent>(_state);
    }

private:
    const size_t _id;
    const tmdl::BlockInterface::ModelInfo _state;

protected:
    class DerivativeComponent : public tmdl::codegen::CodeComponent
    {
    public:
        explicit DerivativeComponent(const tmdl::BlockInterface::ModelInfo& s) : _state(s)
        {
            // Empty Constructor
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_in", {"input_value", "reset_flag"});
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_out", {"output_value"});
        }

        std::string get_module_name() const override
        {
            return "tmdlstd/tmdlstd.hpp";
        }

        std::string get_name_base() const override
        {
            return "derivative_block";
        }

        std::string get_type_name() const override
        {
            return fmt::format("tmdl::stdlib::derivative_block<{}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT));
        }

        std::optional<std::string> get_function_name(const tmdl::codegen::BlockFunction fcn) const override
        {
            switch (fcn)
            {
            case tmdl::codegen::BlockFunction::INIT:
                return "init";
            case tmdl::codegen::BlockFunction::STEP:
                return "step";
            case tmdl::codegen::BlockFunction::RESET:
                return "reset";
            default:
                return {};
            }
        }

        std::vector<std::string> constructor_arguments() const override
        {
            return { std::to_string(_state.get_dt()) };
        }

    private:
        const tmdl::BlockInterface::ModelInfo _state;
    };

    class DerivativeExecutor : public tmdl::BlockExecutionInterface
    {
    public:
        using type_t = typename tmdl::data_type_t<DT>::type;

        explicit DerivativeExecutor(
            std::shared_ptr<const tmdl::ModelValue> input,
            std::shared_ptr<tmdl::ModelValue> output,
            std::shared_ptr<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> reset_flag,
                const tmdl::BlockInterface::ModelInfo& s) :
            _input(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(input)),
            _reset_flag(reset_flag),
            _output(std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(output)),
            state(s)
        {
            if (_input == nullptr || _output == nullptr || _reset_flag == nullptr)
            {
                throw tmdl::ModelException("input parameters are null");
            }
        }

        void init() override
        {
            block = std::make_unique<tmdl::stdlib::derivative_block<type_t>>(state.get_dt());

            update_inputs();
            block->init();
        }

        void step() override
        {
            update_inputs();
            block->step();
            _output->value = block->s_out.output_value;
        }

        void reset() override
        {
            update_inputs();
            block->reset();
        }

        void close() override
        {
            block = nullptr;
        }

    protected:
        void update_inputs()
        {
            block->s_in.input_value = _input->value;
            block->s_in.reset_flag = _reset_flag->value;
        }

    private:
        std::shared_ptr<const tmdl::ModelValueBox<DT>> _input;
        std::shared_ptr<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> _reset_flag;
        std::shared_ptr<tmdl::ModelValueBox<DT>> _output;

        std::unique_ptr<tmdl::stdlib::derivative_block<type_t>> block;
        const tmdl::BlockInterface::ModelInfo state;
    };
};


tmdl::blocks::Derivative::Derivative()
{
    input_type = DataType::UNKNOWN;
    input_reset_flag_type = DataType::UNKNOWN;
}

std::string tmdl::blocks::Derivative::get_name() const
{
    return "derivative";
}

std::string tmdl::blocks::Derivative::get_description() const
{
    return "derives the provided values";
}

size_t tmdl::blocks::Derivative::get_num_inputs() const
{
    return 2;
}

size_t tmdl::blocks::Derivative::get_num_outputs() const
{
    return 1;
}

bool tmdl::blocks::Derivative::update_block()
{
    if (output_port != input_type)
    {
        output_port = input_type;
        return true;
    }

    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::Derivative::has_error() const
{
    if (input_type != DataType::DOUBLE && input_type != DataType::SINGLE)
    {
        return make_error(fmt::format("derivative only works with floating point types, not {}", data_type_to_string(input_type)));
    }
    if (input_type != output_port)
    {
        return make_error("input port doesn't match output port type");
    }
    else if (input_reset_flag_type != DataType::BOOLEAN)
    {
        return make_error("reset flag must be a boolean type");
    }

    return nullptr;
}

void tmdl::blocks::Derivative::set_input_type(
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
    default:
        throw ModelException("input port out of range");
    }
}

tmdl::DataType tmdl::blocks::Derivative::get_output_type(const size_t port) const
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

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::Derivative::get_compiled(const ModelInfo& s) const
{
    if (const auto err = has_error())
    {
        throw ModelException("cannot create componentwith an error");
    }

    switch (input_type)
    {
    case DataType::DOUBLE:
        return std::make_unique<CompiledDerivative<DataType::DOUBLE>>(get_id(), s);
    case DataType::SINGLE:
        return std::make_unique<CompiledDerivative<DataType::SINGLE>>(get_id(), s);
    default:
        throw ModelException("unable to create pointer value");
    }
}
