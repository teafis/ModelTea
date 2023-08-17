// SPDX-License-Identifier: GPL-3.0-only

#include "integrator.hpp"

#include <memory>

#include "../model_exception.hpp"

#include <tmdlstd/tmdlstd.hpp>

#include <fmt/format.h>

template <tmdl::DataType DT>
class CompiledIntegrator : public tmdl::CompiledBlockInterface
{
public:
    CompiledIntegrator(const size_t id, const tmdl::BlockInterface::ModelInfo& s) : _id{ id }, _state(s)
    {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(
        const tmdl::ConnectionManager& connections,
        const tmdl::VariableManager& manager) const override
    {
        const auto in_value = manager.get_ptr(*connections.get_connection_to(_id, 0));
        const auto in_reset_flag = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>>(manager.get_ptr(*connections.get_connection_to(_id, 1)));
        const auto in_reset_value = manager.get_ptr(*connections.get_connection_to(_id, 2));

        const auto out_value = manager.get_ptr(tmdl::VariableIdentifier {
            .block_id = _id,
            .output_port_num = 0
        });

        return std::make_shared<IntegratorExecutor>(in_value, in_reset_value, in_reset_flag, out_value, _state);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override
    {
        return std::make_unique<IntegratorComponent>(_state);
    }

protected:
    const size_t _id;
    const tmdl::BlockInterface::ModelInfo _state;

protected:
    class IntegratorComponent : public tmdl::codegen::CodeComponent
    {
    public:
        IntegratorComponent(const tmdl::BlockInterface::ModelInfo& s) : _state(s)
        {
            // Empty Constructor
        }

        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_in", {"input_value", "reset_value", "reset_flag"});
        }

        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_out", {"output_value"});
        }

        virtual std::string get_module_name() const override
        {
            return "tmdlstd/tmdlstd.hpp";
        }

        virtual std::string get_name_base() const override
        {
            return "integrator_block";
        }

        virtual std::string get_type_name() const override
        {
            return fmt::format("tmdl::stdlib::integrator_block<{}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT));
        }

        virtual std::optional<std::string> get_function_name(const tmdl::codegen::BlockFunction fcn) const override
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

        virtual std::vector<std::string> constructor_arguments() const override
        {
            return { std::to_string(_state.get_dt()) };
        }

        const tmdl::BlockInterface::ModelInfo _state;
    };

    struct IntegratorExecutor : public tmdl::BlockExecutionInterface
    {
        static_assert(tmdl::data_type_t<DT>::is_numeric);
        using type_t = typename tmdl::data_type_t<DT>::type;

        IntegratorExecutor(
            std::shared_ptr<const tmdl::ModelValue> input,
            std::shared_ptr<const tmdl::ModelValue> reset_value,
            std::shared_ptr<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> reset_flag,
            std::shared_ptr<tmdl::ModelValue> output,
                const tmdl::BlockInterface::ModelInfo& s) :
            _input(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(input)),
            _reset_value(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(reset_value)),
            _output(std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(output)),
            _reset_flag(reset_flag),
            state(s)
        {
            if (_input == nullptr || _reset_value == nullptr || _reset_flag == nullptr || _output == nullptr)
            {
                throw tmdl::ModelException("all pointers must be non-null");
            }
        }

        void init() override
        {
            block = std::make_unique<tmdl::stdlib::integrator_block<type_t>>(state.get_dt());

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
            block->s_in.reset_value = _reset_value->value;
            block->s_in.reset_flag = _reset_flag->value;
        }

    protected:
        std::shared_ptr<const tmdl::ModelValueBox<DT>> _input;
        std::shared_ptr<const tmdl::ModelValueBox<DT>> _reset_value;
        std::shared_ptr<tmdl::ModelValueBox<DT>> _output;

        std::shared_ptr<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> _reset_flag;

        std::unique_ptr<tmdl::stdlib::integrator_block<type_t>> block;

        const tmdl::BlockInterface::ModelInfo state;
    };
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

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::Integrator::get_compiled(const ModelInfo& s) const
{
    const auto err = has_error();
    if (err != nullptr)
    {
        throw ModelException("cannot creator interface with an error");
    }

    switch (input_type)
    {
    case DataType::DOUBLE:
        return std::make_unique<CompiledIntegrator<DataType::DOUBLE>>(get_id(), s);
    case DataType::SINGLE:
        return std::make_unique<CompiledIntegrator<DataType::SINGLE>>(get_id(), s);
    default:
        throw ModelException("unable to create pointer value");
    }
}


