// SPDX-License-Identifier: GPL-3.0-only

#include "constant.hpp"

#include "../model_exception.hpp"

#include <tmdlstd/tmdlstd.hpp>
#include <fmt/format.h>


tmdl::blocks::Constant::Constant() :
    param_dtype(std::make_shared<Parameter>("dtype", "data type", ParameterValue::from_string("", ParameterValue::Type::DATA_TYPE))),
    param_value(std::make_shared<Parameter>("value", "Constant Value", ParameterValue::from_string("", ParameterValue::Type::UNKNOWN)))
{
    // Empty Constructor
}

std::string tmdl::blocks::Constant::get_name() const
{
    return "constant";
}

std::string tmdl::blocks::Constant::get_description() const
{
    return "Provides a constant value output";
}

size_t tmdl::blocks::Constant::get_num_inputs() const
{
    return 0;
}

size_t tmdl::blocks::Constant::get_num_outputs() const
{
    return 1;
}

bool tmdl::blocks::Constant::update_block()
{
    if (param_dtype->get_value().value.dtype != output_port)
    {
        output_port = param_dtype->get_value().value.dtype;

        std::ostringstream oss;
        oss << "Constant Value (" << data_type_to_string(output_port) << ")";

        param_value->set_name(oss.str());
        param_value->get_value().convert(output_port);

        return true;
    }

    return false;
}

std::vector<std::shared_ptr<tmdl::Parameter>> tmdl::blocks::Constant::get_parameters() const
{
    return {
        param_dtype,
        param_value
    };
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::Constant::has_error() const
{
    if (param_dtype->get_value().value.dtype == DataType::UNKNOWN)
    {
        return make_error("data type provided is of unknown type");
    }

    return nullptr;
}

void tmdl::blocks::Constant::set_input_type(
    const size_t,
    const DataType)
{
    throw ModelException("input port out of range");
}

tmdl::DataType tmdl::blocks::Constant::get_output_type(const size_t port) const
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

template <tmdl::DataType DT>
class CompiledConstant : public tmdl::CompiledBlockInterface
{
public:
    using type_t = typename tmdl::data_type_t<DT>::type;

public:
    explicit CompiledConstant(const size_t id, const type_t value) : _id{ id }, _value{ value }
    {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(
        const tmdl::ConnectionManager&,
        const tmdl::VariableManager& manager) const override
    {
        const auto output_type = manager.get_ptr(tmdl::VariableIdentifier
        {
            .block_id = _id,
            .output_port_num = 0
        });

        return std::make_shared<ConstantExecutor>(
            _value,
            output_type);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override
    {
        return std::make_unique<ConstantComponent>();
    }

protected:
    const size_t _id;
    const type_t _value;

protected:
    struct ConstantComponent : public tmdl::codegen::CodeComponent
    {
        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
        {
            return {};
        }

        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_out", {"val"});
        }

        virtual std::string get_module_name() const override
        {
            return "tmdlstd/tmdlstd.hpp";
        }

        virtual std::string get_name_base() const override
        {
            return "const_block";
        }

        virtual std::string get_type_name() const override
        {
            return fmt::format("tmdlstd::const_block<{}>", tmdl::data_type_to_string(DT));
        }

        virtual std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction) const override
        {
            return {};
        }
    };

    struct ConstantExecutor : public tmdl::BlockExecutionInterface
    {
        explicit ConstantExecutor(
            const type_t& value,
            const std::shared_ptr<tmdl::ModelValue> ptr)
        {
            const auto ptr_type = std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(ptr);
            if (ptr == nullptr)
            {
                throw tmdl::ModelException("provided output pointer cannot be null");
            }

            block = std::make_unique<tmdl::stdlib::const_block<type_t>>(value);

            ptr_type->value = block->s_out.val;
        }

        std::unique_ptr<tmdl::stdlib::const_block<type_t>> block;
    };
};

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::Constant::get_compiled() const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot build constant executor with error present");
    }

    switch (param_dtype->get_value().value.dtype)
    {
    case DataType::DOUBLE:
        return std::make_unique<CompiledConstant<DataType::DOUBLE>>(get_id(), param_value->get_value().value.f64);
    case DataType::SINGLE:
        return std::make_unique<CompiledConstant<DataType::SINGLE>>(get_id(), param_value->get_value().value.f32);
    case DataType::BOOLEAN:
        return std::make_unique<CompiledConstant<DataType::BOOLEAN>>(get_id(), param_value->get_value().value.tf);
    case DataType::INT32:
        return std::make_unique<CompiledConstant<DataType::INT32>>(get_id(), param_value->get_value().value.i32);
    case DataType::UINT32:
        return std::make_unique<CompiledConstant<DataType::UINT32>>(get_id(), param_value->get_value().value.u32);
    default:
        throw ModelException("unknown data type provided for executor");
    }
}
