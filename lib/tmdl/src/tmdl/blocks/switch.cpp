// SPDX-License-Identifier: GPL-3.0-only

#include "switch.hpp"

#include "../model_exception.hpp"

#include <algorithm>

#include <fmt/format.h>

#include <tmdlstd/tmdlstd.hpp>

using namespace tmdl;
using namespace tmdl::blocks;

template <tmdl::DataType DT>
class CompiledSwitch : public tmdl::CompiledBlockInterface
{
protected:
    using limit_t = typename tmdl::data_type_t<DT>::type;

public:
    explicit CompiledSwitch(const size_t id) :
        _id{ id }
    {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(
        const tmdl::ConnectionManager& connections,
        const tmdl::VariableManager& manager) const override
    {
        const auto in_tf = manager.get_ptr(*connections.get_connection_to(_id, 0));
        const auto in_a = manager.get_ptr(*connections.get_connection_to(_id, 1));
        const auto in_b = manager.get_ptr(*connections.get_connection_to(_id, 2));

        const auto vidOutput = VariableIdentifier {
            .block_id = _id,
            .output_port_num = 0
        };

        const auto outputPointer = manager.get_ptr(vidOutput);

        return std::make_shared<SwitchExecutor>(
            in_tf,
            in_a,
            in_b,
            outputPointer);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override
    {
        return std::make_unique<SwitchComponent>();
    }

private:
    const size_t _id;

protected:
    struct SwitchComponent : public tmdl::codegen::CodeComponent
    {
        std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_in", {"switch_value", "value_a", "value_b"});
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_out", {"value"});
        }

        std::string get_module_name() const override
        {
            return "tmdlstd/tmdlstd.hpp";
        }

        std::string get_name_base() const override
        {
            return "switch_block";
        }

        std::string get_type_name() const override
        {
            return fmt::format("tmdl::stdlib::switch_block<{}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT));
        }

        std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override
        {
            switch (ft)
            {
            case tmdl::codegen::BlockFunction::INIT:
                return "init";
            case tmdl::codegen::BlockFunction::STEP:
                return "step";
            default:
                return {};
            }
        }
    };

    class SwitchExecutor : public BlockExecutionInterface
    {
    public:
        SwitchExecutor(
            std::shared_ptr<const ModelValue> ptr_tf,
            std::shared_ptr<const ModelValue> ptr_a,
            std::shared_ptr<const ModelValue> ptr_b,
            std::shared_ptr<ModelValue> ptr_output) :
            _ptr_switch(std::dynamic_pointer_cast<const ModelValueBox<DataType::BOOLEAN>>(ptr_tf)),
            _ptr_output(std::dynamic_pointer_cast<ModelValueBox<DT>>(ptr_output)),
            _ptr_val_a(std::dynamic_pointer_cast<const ModelValueBox<DT>>(ptr_a)),
            _ptr_val_b(std::dynamic_pointer_cast<const ModelValueBox<DT>>(ptr_b))
        {
            if (_ptr_switch == nullptr || _ptr_output == nullptr || _ptr_val_a == nullptr || _ptr_val_b == nullptr)
            {
                throw ModelException("input pointers cannot be null");
            }
        }

        void init() override
        {
            block.s_in.switch_value = _ptr_switch->value;
            block.s_in.value_a = _ptr_val_a->value;
            block.s_in.value_b = _ptr_val_b->value;
            block.init();
            _ptr_output->value = block.s_out.value;
        }

        void step() override
        {
            block.s_in.switch_value = _ptr_switch->value;
            block.s_in.value_a = _ptr_val_a->value;
            block.s_in.value_b = _ptr_val_b->value;
            block.step();
            _ptr_output->value = block.s_out.value;
        }

    private:
        std::shared_ptr<const ModelValueBox<DataType::BOOLEAN>> _ptr_switch;
        std::shared_ptr<ModelValueBox<DT>> _ptr_output;
        std::shared_ptr<const ModelValueBox<DT>> _ptr_val_a;
        std::shared_ptr<const ModelValueBox<DT>> _ptr_val_b;

        tmdl::stdlib::switch_block<limit_t> block;
    };
};

Switch::Switch()
{
    input_type_tf = DataType::UNKNOWN;
    input_type_a = DataType::UNKNOWN;
    input_type_b = DataType::UNKNOWN;
    output_port = DataType::UNKNOWN;
}

std::string Switch::get_name() const
{
    return "switch";
}

std::string Switch::get_description() const
{
    return "Sets the output to A when TF is true, or B when TF is false.";
}

size_t Switch::get_num_inputs() const
{
    return 3;
}

size_t Switch::get_num_outputs() const
{
    return 1;
}

bool Switch::update_block()
{
    bool updated = false;

    if (input_type_a != output_port)
    {
        output_port = input_type_a;
        updated = true;
    }

    return updated;
}

std::unique_ptr<const BlockError> Switch::has_error() const
{
    // Check input types
    if (input_type_tf != DataType::BOOLEAN)
    {
        return make_error("tf input type must be a bookean");
    }
    else if (input_type_a != input_type_b)
    {
        return make_error("input a and b must be the same type");
    }
    else if (input_type_a == DataType::UNKNOWN)
    {
        return make_error("input type cannot be unknown");
    }
    else
    {
        return nullptr;
    }
}

void Switch::set_input_type(
    const size_t port,
    const DataType type)
{
    if (port < get_num_inputs())
    {
        switch (port)
        {
        case 0:
            input_type_tf = type;
            break;
        case 1:
            input_type_a = type;
            break;
        case 2:
            input_type_b = type;
            break;
        default:
        throw ModelException("provided input port too high");
        }
    }
    else
    {
        throw ModelException("provided input port too high");
    }
}

DataType Switch::get_output_type(const size_t port) const
{
    if (port < get_num_outputs())
    {
        return output_port;
    }
    else
    {
        throw ModelException("provided output port is too high");
    }
}

std::unique_ptr<CompiledBlockInterface> Switch::get_compiled(const ModelInfo&) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot execute switch with incomplete input parameters");
    }

    switch (output_port)
    {
    case DataType::DOUBLE:
        return std::make_unique<CompiledSwitch<DataType::DOUBLE>>(get_id());
    case DataType::SINGLE:
        return std::make_unique<CompiledSwitch<DataType::SINGLE>>(get_id());
    case DataType::INT32:
        return std::make_unique<CompiledSwitch<DataType::INT32>>(get_id());
    case DataType::UINT32:
        return std::make_unique<CompiledSwitch<DataType::UINT32>>(get_id());
    case DataType::BOOLEAN:
        return std::make_unique<CompiledSwitch<DataType::BOOLEAN>>(get_id());
    default:
        throw ModelException("unable to generate limitor executor");
    }
}
