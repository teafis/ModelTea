// SPDX-License-Identifier: GPL-3.0-only

#include "value.hpp"
#include "../model_exception.hpp"

#include <unordered_map>
#include <vector>
#include <utility>

class DataTypeMap
{
protected:
    DataTypeMap()
    {
        const std::vector<std::pair<tmdl::DataType, std::string>> type_vals{
            { tmdl::DataType::BOOLEAN, "bool" },
            { tmdl::DataType::DOUBLE, "f64" },
            { tmdl::DataType::SINGLE, "f32" },
            { tmdl::DataType::INT32, "i32" },
            { tmdl::DataType::UINT32, "u32" },
        };

        for (const auto& [t, n] : type_vals)
        {
            name_to_type.insert({n, t});
            type_to_name.insert({t, n});
        }
    }

public:
    static const DataTypeMap& get_instance()
    {
        static DataTypeMap inst;
        return inst;
    }

    tmdl::DataType get_type(const std::string& name) const
    {
        const auto it = name_to_type.find(name);
        if (it != name_to_type.end())
        {
            return it->second;
        }
        else
        {
            return tmdl::DataType::UNKNOWN;
        }
    }

    const std::string& get_name(const tmdl::DataType dt) const
    {
        const auto it = type_to_name.find(dt);
        if (it != type_to_name.end())
        {
            return it->second;
        }
        else
        {
            const static std::string unknown_str = "unknown";
            return unknown_str;
        }
    }

protected:
    std::unordered_map<std::string, tmdl::DataType> name_to_type;
    std::unordered_map<tmdl::DataType, std::string> type_to_name;
};

std::string tmdl::data_type_to_string(const DataType dtype)
{
    return DataTypeMap::get_instance().get_name(dtype);
}

tmdl::DataType tmdl::data_type_from_string(const std::string& s)
{
    return DataTypeMap::get_instance().get_type(s);
}

tmdl::ModelValue* tmdl::ModelValue::make_default_type(const DataType dtype)
{
    switch (dtype)
    {
    case tmdl::DataType::BOOLEAN:
        return make_default<tmdl::DataType::BOOLEAN>();
    case tmdl::DataType::DOUBLE:
        return make_default<tmdl::DataType::DOUBLE>();
    case tmdl::DataType::SINGLE:
        return make_default<tmdl::DataType::SINGLE>();
    case tmdl::DataType::INT32:
        return make_default<tmdl::DataType::INT32>();
    case tmdl::DataType::UINT32:
        return make_default<tmdl::DataType::UINT32>();
    default:
        throw ModelException("unable to construct value for type");
    }
}

tmdl::ModelValue* tmdl::ModelValue::from_string(const std::string& s, const DataType dt)
{
    try
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return new ModelValueBox<DataType::BOOLEAN>(std::stoi(s) != 0);
        case DataType::INT32:
            return new ModelValueBox<DataType::INT32>(std::stoi(s));
        case DataType::UINT32:
            return new ModelValueBox<DataType::UINT32>(std::stoul(s));
        case DataType::SINGLE:
            return new ModelValueBox<DataType::SINGLE>(std::stof(s));
        case DataType::DOUBLE:
            return new ModelValueBox<DataType::DOUBLE>(std::stod(s));
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

tmdl::ModelValue* tmdl::ModelValue::convert_type(const ModelValue* val, const DataType dt)
{
    if (auto ptr = dynamic_cast<const ModelValueBox<DataType::BOOLEAN>*>(val))
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return new ModelValueBox<DataType::BOOLEAN>(ptr->value);
        case DataType::INT32:
            return new ModelValueBox<DataType::INT32>(ptr->value);
        case DataType::UINT32:
            return new ModelValueBox<DataType::UINT32>(ptr->value);
        case DataType::SINGLE:
            return new ModelValueBox<DataType::SINGLE>(ptr->value);
        case DataType::DOUBLE:
            return new ModelValueBox<DataType::DOUBLE>(ptr->value);
        default:
            throw ModelException("unknown data type provided");
        }
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::INT32>*>(val))
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return new ModelValueBox<DataType::BOOLEAN>(ptr->value != 0);
        case DataType::INT32:
            return new ModelValueBox<DataType::INT32>(ptr->value);
        case DataType::UINT32:
            return new ModelValueBox<DataType::UINT32>(ptr->value);
        case DataType::SINGLE:
            return new ModelValueBox<DataType::SINGLE>(ptr->value);
        case DataType::DOUBLE:
            return new ModelValueBox<DataType::DOUBLE>(ptr->value);
        default:
            throw ModelException("unknown data type provided");
        }
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::UINT32>*>(val))
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return new ModelValueBox<DataType::BOOLEAN>(ptr->value != 0);
        case DataType::INT32:
            return new ModelValueBox<DataType::INT32>(ptr->value);
        case DataType::UINT32:
            return new ModelValueBox<DataType::UINT32>(ptr->value);
        case DataType::SINGLE:
            return new ModelValueBox<DataType::SINGLE>(ptr->value);
        case DataType::DOUBLE:
            return new ModelValueBox<DataType::DOUBLE>(ptr->value);
        default:
            throw ModelException("unknown data type provided");
        }
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::SINGLE>*>(val))
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return new ModelValueBox<DataType::BOOLEAN>(ptr->value != 0);
        case DataType::INT32:
            return new ModelValueBox<DataType::INT32>(ptr->value);
        case DataType::UINT32:
            return new ModelValueBox<DataType::UINT32>(ptr->value);
        case DataType::SINGLE:
            return new ModelValueBox<DataType::SINGLE>(ptr->value);
        case DataType::DOUBLE:
            return new ModelValueBox<DataType::DOUBLE>(ptr->value);
        default:
            throw ModelException("unknown data type provided");
        }
    }
    else if (auto ptr = dynamic_cast<const ModelValueBox<DataType::DOUBLE>*>(val))
    {
        switch (dt)
        {
        case DataType::BOOLEAN:
            return new ModelValueBox<DataType::BOOLEAN>(ptr->value != 0);
        case DataType::INT32:
            return new ModelValueBox<DataType::INT32>(ptr->value);
        case DataType::UINT32:
            return new ModelValueBox<DataType::UINT32>(ptr->value);
        case DataType::SINGLE:
            return new ModelValueBox<DataType::SINGLE>(ptr->value);
        case DataType::DOUBLE:
            return new ModelValueBox<DataType::DOUBLE>(ptr->value);
        default:
            throw ModelException("unknown data type provided");
        }
    }
    else
    {
        throw ModelException("unknown input value type");
    }
}
