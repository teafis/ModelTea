// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/value.hpp>
#include <tmdl/model_exception.hpp>

std::string tmdl::data_type_to_string(const DataType dtype)
{
    switch (dtype)
    {
    case DataType::BOOLEAN:
        return "bool";
    case DataType::DOUBLE:
        return "f64";
    case DataType::SINGLE:
        return "f32";
    case DataType::INT32:
        return "i32";
    case DataType::UINT32:
        return "u32";
    default:
        return "unknown";
    }
}

tmdl::DataType tmdl::data_type_from_string(const std::string& s)
{
    if (s == "i32")
    {
        return DataType::INT32;
    }
    else if (s == "u32")
    {
        return DataType::UINT32;
    }
    else if (s == "f64")
    {
        return DataType::DOUBLE;
    }
    else if (s == "f32")
    {
        return DataType::SINGLE;
    }
    else if (s == "bool")
    {
        return DataType::BOOLEAN;
    }
    else
    {
        return DataType::UNKNOWN;
    }
}

template <typename T>
static std::shared_ptr<tmdl::ValueBox> get_default_shared()
{
    return std::make_shared<tmdl::ValueBoxType<T>>(T{});
}

std::shared_ptr<tmdl::ValueBox> tmdl::make_shared_default_value(const DataType dtype)
{
    switch (dtype)
    {
    case DataType::BOOLEAN:
        return get_default_shared<bool>();
    case DataType::SINGLE:
        return get_default_shared<float>();
    case DataType::DOUBLE:
        return get_default_shared<double>();
    case DataType::INT32:
        return get_default_shared<int32_t>();
    case DataType::UINT32:
        return get_default_shared<uint32_t>();
    default:
        throw ModelException("unable to construct value for type");
    }
}
