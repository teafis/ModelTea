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
    default:
        throw ModelException("unable to construct value for type");
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

std::unique_ptr<tmdl::ModelValue> tmdl::ModelValue::convert_type(const ModelValue* val, const DataType dt)
{
    if (val == nullptr) throw ModelException("unexpected nullptr");

    if (auto ptr = dynamic_cast<const ModelValueBox<DataType::BOOLEAN>*>(val))
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return std::make_unique<ModelValueBox<DataType::BOOLEAN>>(ptr->value);
        case DataType::INT32:
            return std::make_unique<ModelValueBox<DataType::INT32>>(ptr->value);
        case DataType::UINT32:
            return std::make_unique<ModelValueBox<DataType::UINT32>>(ptr->value);
        case DataType::SINGLE:
            return std::make_unique<ModelValueBox<DataType::SINGLE>>(ptr->value);
        case DataType::DOUBLE:
            return std::make_unique<ModelValueBox<DataType::DOUBLE>>(ptr->value);
        default:
            throw ModelException("unknown data type provided");
        }
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::INT32>*>(val))
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return std::make_unique<ModelValueBox<DataType::BOOLEAN>>(ptr->value != 0);
        case DataType::INT32:
            return std::make_unique<ModelValueBox<DataType::INT32>>(ptr->value);
        case DataType::UINT32:
            return std::make_unique<ModelValueBox<DataType::UINT32>>(ptr->value);
        case DataType::SINGLE:
            return std::make_unique<ModelValueBox<DataType::SINGLE>>(ptr->value);
        case DataType::DOUBLE:
            return std::make_unique<ModelValueBox<DataType::DOUBLE>>(ptr->value);
        default:
            throw ModelException("unknown data type provided");
        }
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::UINT32>*>(val))
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return std::make_unique<ModelValueBox<DataType::BOOLEAN>>(ptr->value != 0);
        case DataType::INT32:
            return std::make_unique<ModelValueBox<DataType::INT32>>(ptr->value);
        case DataType::UINT32:
            return std::make_unique<ModelValueBox<DataType::UINT32>>(ptr->value);
        case DataType::SINGLE:
            return std::make_unique<ModelValueBox<DataType::SINGLE>>(ptr->value);
        case DataType::DOUBLE:
            return std::make_unique<ModelValueBox<DataType::DOUBLE>>(ptr->value);
        default:
            throw ModelException("unknown data type provided");
        }
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::SINGLE>*>(val))
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return std::make_unique<ModelValueBox<DataType::BOOLEAN>>(ptr->value != 0);
        case DataType::INT32:
            return std::make_unique<ModelValueBox<DataType::INT32>>(ptr->value);
        case DataType::UINT32:
            return std::make_unique<ModelValueBox<DataType::UINT32>>(ptr->value);
        case DataType::SINGLE:
            return std::make_unique<ModelValueBox<DataType::SINGLE>>(ptr->value);
        case DataType::DOUBLE:
            return std::make_unique<ModelValueBox<DataType::DOUBLE>>(ptr->value);
        default:
            throw ModelException("unknown data type provided");
        }
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::DOUBLE>*>(val))
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return std::make_unique<ModelValueBox<DataType::BOOLEAN>>(ptr->value != 0);
        case DataType::INT32:
            return std::make_unique<ModelValueBox<DataType::INT32>>(ptr->value);
        case DataType::UINT32:
            return std::make_unique<ModelValueBox<DataType::UINT32>>(ptr->value);
        case DataType::SINGLE:
            return std::make_unique<ModelValueBox<DataType::SINGLE>>(ptr->value);
        case DataType::DOUBLE:
            return std::make_unique<ModelValueBox<DataType::DOUBLE>>(ptr->value);
        default:
            throw ModelException("unknown data type provided");
        }
    }
    else
    {
        throw ModelException("unknown input value type");
    }
}
