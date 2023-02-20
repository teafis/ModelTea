// SPDX-License-Identifier: GPL-3.0-only

#include "constant.hpp"

#include "../model_exception.hpp"

#include <sstream>


tmdl::stdlib::Constant::Constant() :
    param_dtype(std::make_shared<Parameter>("dtype", "data type", ParameterValue::from_string("", ParameterValue::Type::DATA_TYPE))),
    param_value(std::make_shared<Parameter>("value", "Constant Value", ParameterValue::from_string("", ParameterValue::Type::UNKNOWN)))
{
    // Empty Constructor
}

std::string tmdl::stdlib::Constant::get_name() const
{
    return "constant";
}

std::string tmdl::stdlib::Constant::get_description() const
{
    return "Provides a constant value output";
}

size_t tmdl::stdlib::Constant::get_num_inputs() const
{
    return 0;
}

size_t tmdl::stdlib::Constant::get_num_outputs() const
{
    return 1;
}

bool tmdl::stdlib::Constant::update_block()
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

std::vector<std::shared_ptr<tmdl::Parameter>> tmdl::stdlib::Constant::get_parameters() const
{
    return {
        param_dtype,
        param_value
    };
}

std::unique_ptr<const tmdl::BlockError> tmdl::stdlib::Constant::has_error() const
{
    if (param_dtype->get_value().value.dtype == DataType::UNKNOWN)
    {
        return make_error("data type provided is of unknown type");
    }

    return nullptr;
}

void tmdl::stdlib::Constant::set_input_type(
    const size_t,
    const DataType)
{
    throw ModelException("input port out of range");
}

tmdl::DataType tmdl::stdlib::Constant::get_output_type(const size_t port) const
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
struct ConstantExecutor : public tmdl::BlockExecutionInterface
{
    using type_t = tmdl::data_type_t<DT>::type;

    ConstantExecutor(
        const type_t& value,
        const std::shared_ptr<tmdl::ModelValue> ptr)
    {
        const auto ptr_type = std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(ptr);
        if (ptr == nullptr)
        {
            throw tmdl::ModelException("provided output pointer cannot be null");
        }

        ptr_type->value = value;
    }
};

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Constant::get_execution_interface(
    const ConnectionManager&,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot build executor with error present");
    }

    const auto output_type = manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (param_dtype->get_value().value.dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<ConstantExecutor<DataType::DOUBLE>>(
            param_value->get_value().value.f64,
            output_type);
    case DataType::SINGLE:
        return std::make_shared<ConstantExecutor<DataType::SINGLE>>(
            param_value->get_value().value.f32,
            output_type);
    case DataType::BOOLEAN:
        return std::make_shared<ConstantExecutor<DataType::BOOLEAN>>(
            param_value->get_value().value.tf,
            output_type);
    case DataType::INT32:
        return std::make_shared<ConstantExecutor<DataType::INT32>>(
            param_value->get_value().value.i32,
            output_type);
    case DataType::UINT32:
        return std::make_shared<ConstantExecutor<DataType::UINT32>>(
            param_value->get_value().value.u32,
            output_type);
    default:
        throw ModelException("unknown data type provided for executor");
    }
}
