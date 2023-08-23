// SPDX-License-Identifier: GPL-3.0-only

#include "value.hpp"
#include "../model_exception.hpp"

#include <utility>

template <tmdl::DataType DT>
static std::unique_ptr<tmdl::ModelValue> make_default_static()
{
    return std::make_unique<tmdl::ModelValueBox<DT>>();
}

std::unique_ptr<tmdl::ModelValue> tmdl::ModelValue::make_default(const DataType dtype)
{
    switch (dtype)
    {
    case tmdl::DataType::BOOLEAN:
        return make_default_static<tmdl::DataType::BOOLEAN>();
    case tmdl::DataType::DOUBLE:
        return make_default_static<tmdl::DataType::DOUBLE>();
    case tmdl::DataType::SINGLE:
        return make_default_static<tmdl::DataType::SINGLE>();
    case tmdl::DataType::INT32:
        return make_default_static<tmdl::DataType::INT32>();
    case tmdl::DataType::UINT32:
        return make_default_static<tmdl::DataType::UINT32>();
    case tmdl::DataType::DATA_TYPE:
        return make_default_static<tmdl::DataType::DATA_TYPE>();
    case tmdl::DataType::UNKNOWN:
        return make_default_static<tmdl::DataType::UNKNOWN>();
    default:
        throw ModelException(fmt::format("unable to construct value for type {}", data_type_to_string(dtype)));
    }
}

std::unique_ptr<tmdl::ModelValue> tmdl::ModelValue::from_string(const std::string& s, const DataType dt)
{
    try
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return std::make_unique<ModelValueBox<DataType::BOOLEAN>>(std::stoi(s) != 0);
        case DataType::INT32:
            return std::make_unique<ModelValueBox<DataType::INT32>>(std::stoi(s));
        case DataType::UINT32:
            return std::make_unique<ModelValueBox<DataType::UINT32>>(std::stoul(s));
        case DataType::SINGLE:
            return std::make_unique<ModelValueBox<DataType::SINGLE>>(std::stof(s));
        case DataType::DOUBLE:
            return std::make_unique<ModelValueBox<DataType::DOUBLE>>(std::stod(s));
        case DataType::DATA_TYPE:
            return std::make_unique<ModelValueBox<DataType::DATA_TYPE>>(data_type_from_string(s));
        case DataType::IDENTIFIER:
            return std::make_unique<ModelValueBox<DataType::IDENTIFIER>>(Identifier(s));
        case DataType::UNKNOWN:
            return make_default_static<tmdl::DataType::UNKNOWN>();
        default:
            throw ModelException("unknown parse parameter type provided");
        }
    }
    catch (const std::invalid_argument&)
    {
        throw ModelException("error parsing parameter - invalid argument");
    }
    catch (const std::out_of_range&)
    {
        throw ModelException("error parsing parameter - out of range");
    }
}

template <tmdl::DataType DT>
static std::unique_ptr<tmdl::ModelValue> convert_numeric_type_helper(const tmdl::ModelValueBox<DT>* ptr, const tmdl::DataType dt)
{
    switch (dt)
    {
    case tmdl::DataType::BOOLEAN:
        if constexpr (DT == tmdl::DataType::BOOLEAN)
        {
            return std::make_unique<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>>(ptr->value);
        }
        else
        {
            return std::make_unique<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>>(ptr->value != 0);
        }
    case tmdl::DataType::INT32:
        return std::make_unique<tmdl::ModelValueBox<tmdl::DataType::INT32>>(ptr->value);
    case tmdl::DataType::UINT32:
        return std::make_unique<tmdl::ModelValueBox<tmdl::DataType::UINT32>>(ptr->value);
    case tmdl::DataType::SINGLE:
        return std::make_unique<tmdl::ModelValueBox<tmdl::DataType::SINGLE>>(ptr->value);
    case tmdl::DataType::DOUBLE:
        return std::make_unique<tmdl::ModelValueBox<tmdl::DataType::DOUBLE>>(ptr->value);
    default:
        throw tmdl::ModelException("unsupported data type provided");
    }
}

std::unique_ptr<tmdl::ModelValue> tmdl::ModelValue::convert_type(const ModelValue* val, const DataType dt)
{
    if (val == nullptr) throw ModelException("unexpected nullptr");

    if (auto ptr = dynamic_cast<const ModelValueBox<DataType::BOOLEAN>*>(val))
    {
        return convert_numeric_type_helper<DataType::BOOLEAN>(ptr, dt);
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::INT32>*>(val))
    {
        return convert_numeric_type_helper<DataType::INT32>(ptr, dt);
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::UINT32>*>(val))
    {
        return convert_numeric_type_helper<DataType::UINT32>(ptr, dt);
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::SINGLE>*>(val))
    {
        return convert_numeric_type_helper<DataType::SINGLE>(ptr, dt);
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::DOUBLE>*>(val))
    {
        return convert_numeric_type_helper<DataType::DOUBLE>(ptr, dt);
    }
    else
    {
        throw ModelException("unsupported input value type");
    }
}
