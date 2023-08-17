// SPDX-License-Identifier: GPL-3.0-only

#include "trig.hpp"

#include "../model_exception.hpp"

#include <tmdlstd/tmdlstd.hpp>

#include <fmt/format.h>


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
class CompiledTrig : public tmdl::CompiledBlockInterface
{
public:
    CompiledTrig(const size_t id) :
        _id{ id }
    {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(
        const tmdl::ConnectionManager& connections,
        const tmdl::VariableManager& manager) const override
    {
        const auto inputValue = manager.get_ptr(*connections.get_connection_to(_id, 0));
        const auto outputValue = manager.get_ptr(tmdl::VariableIdentifier
        {
            .block_id = _id,
            .output_port_num = 0
        });

        return std::make_shared<TrigExecutor>(
            inputValue,
            outputValue);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override
    {
        return std::make_unique<TrigComponent>();
    }

protected:
    const size_t _id;

protected:
    struct TrigComponent : public tmdl::codegen::CodeComponent
    {
        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_in", {"value"});
        }

        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_out", {"value"});
        }

        virtual std::string get_module_name() const override
        {
            return "tmdlstd/tmdlstd.hpp";
        }

        virtual std::string get_name_base() const override
        {
            return "trig_block";
        }

        virtual std::string get_type_name() const override
        {
            return fmt::format("tmdl::stdlib::trig_block<{}, {}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT), tmdl::stdlib::trig_func_to_string(FCN));
        }

        virtual std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override
        {
            switch (ft)
            {
            case tmdl::codegen::BlockFunction::STEP:
                return "step";
            case tmdl::codegen::BlockFunction::INIT:
                return "init";
            default:
                return {};
            }
        }
    };

protected:
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
        }

    public:
        void init() override
        {
            block.s_in.value = _ptr_input->value;
            block.init();
            _ptr_output->value = block.s_out.value;
        }

        void step() override
        {
            block.s_in.value = _ptr_input->value;
            block.step();
            _ptr_output->value = block.s_out.value;
        }

    protected:
        const std::shared_ptr<const tmdl::ModelValueBox<DT>> _ptr_input;
        const std::shared_ptr<tmdl::ModelValueBox<DT>> _ptr_output;
        tmdl::stdlib::trig_block<val_t, FCN> block;
    };
};

template <tmdl::stdlib::TrigFunction FCN>
static std::unique_ptr<tmdl::CompiledBlockInterface> generate_compiler_interface(const tmdl::blocks::TrigFunction* model)
{
    if (model->has_error() != nullptr)
    {
        throw tmdl::ModelException(fmt::format("cannot execute {} with incomplete input parameters", model->get_name()));
    }

    switch (model->get_output_type(0))
    {
    case tmdl::DataType::DOUBLE:
        return std::make_unique<CompiledTrig<tmdl::DataType::DOUBLE, FCN>>(model->get_id());
    case tmdl::DataType::SINGLE:
        return std::make_unique<CompiledTrig<tmdl::DataType::SINGLE, FCN>>(model->get_id());
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

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::TrigSin::get_compiled(const SimState&) const
{
    return generate_compiler_interface<tmdl::stdlib::TrigFunction::SIN>(this);
}

std::string tmdl::blocks::TrigCos::get_name() const
{
    return "cos";
}

std::string tmdl::blocks::TrigCos::get_description() const
{
    return "computes the cos of the input parameter";
}

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::TrigCos::get_compiled(const SimState&) const
{
    return generate_compiler_interface<tmdl::stdlib::TrigFunction::COS>(this);
}
