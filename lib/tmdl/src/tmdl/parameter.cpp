// SPDX-License-Identifier: GPL-3.0-only

#include "parameter.hpp"

#include "model_exception.hpp"

#include <iomanip>


std::string tmdl::ParameterValue::to_string() const
{
    std::ostringstream oss;

    switch (dtype)
    {
    case Type::BOOLEAN:
        oss << (value.tf ? 1 : 0);
        break;
    case Type::SINGLE:
        oss << std::setprecision(20) << value.f32;
        break;
    case Type::DOUBLE:
        oss << std::setprecision(20) << value.f64;
        break;
    case Type::INT32:
        oss << value.i32;
        break;
    case Type::UINT32:
        oss << value.u32;
        break;
    case Type::DATA_TYPE:
        oss << data_type_to_string(value.dtype);
        break;
    default:
        throw ModelException("unsupported type");
    }

    return oss.str();
}

void tmdl::ParameterValue::convert(const Type t)
{
    if (t == dtype)
    {
        return;
    }

    switch (t)
    {
    case Type::BOOLEAN:
    {
        switch (dtype)
        {
        case Type::SINGLE:
            value.tf = value.f32 != 0;
            break;
        case Type::DOUBLE:
            value.tf = value.f64 != 0;
            break;
        case Type::INT32:
            value.tf = value.i32 != 0;
            break;
        case Type::UINT32:
            value.tf = value.u32 != 0;
            break;
        default:
            value.tf = false;
        }
        break;
    }
    case Type::SINGLE:
    {
        switch (dtype)
        {
        case Type::DOUBLE:
            value.f32 = static_cast<float>(value.f64);
            break;
        case Type::INT32:
            value.f32 = static_cast<float>(value.i32);
            break;
        case Type::UINT32:
            value.f32 = static_cast<float>(value.u32);
            break;
        default:
            value.f32 = 0.0f;
        }
        break;
    }
    case Type::DOUBLE:
    {
        switch (dtype)
        {
        case Type::SINGLE:
            value.f64 = static_cast<double>(value.f32);
            break;
        case Type::INT32:
            value.f64 = static_cast<double>(value.i32);
            break;
        case Type::UINT32:
            value.f64 = static_cast<double>(value.u32);
            break;
        default:
            value.f64 = 0.0;
        }
        break;
    }
    case Type::INT32:
    {
        switch (dtype)
        {
        case Type::SINGLE:
            value.i32 = static_cast<int32_t>(value.f32);
            break;
        case Type::DOUBLE:
            value.i32 = static_cast<int32_t>(value.f64);
            break;
        case Type::UINT32:
            value.i32 = static_cast<int32_t>(value.u32);
            break;
        default:
            value.i32 = 0;
        }
        break;
    }
    case Type::UINT32:
    {
        switch (dtype)
        {
        case Type::SINGLE:
            value.u32 = static_cast<uint32_t>(value.f32);
            break;
        case Type::DOUBLE:
            value.u32 = static_cast<uint32_t>(value.f64);
            break;
        case Type::INT32:
            value.u32 = static_cast<uint32_t>(value.i32);
            break;
        default:
            value.u32 = 0;
        }
        break;
    }
    case Type::DATA_TYPE:
        value.dtype = DataType::UNKNOWN;
        break;

    default:
        value = Value{};
    }

    dtype = t;
}

void tmdl::ParameterValue::convert(const DataType t)
{
    switch (t)
    {
    case DataType::DOUBLE:
        convert(Type::DOUBLE);
        break;
    case DataType::SINGLE:
        convert(Type::SINGLE);
        break;
    case DataType::INT32:
        convert(Type::INT32);
        break;
    case DataType::UINT32:
        convert(Type::UINT32);
        break;
    case DataType::BOOLEAN:
        convert(Type::BOOLEAN);
        break;
    default:
        convert(Type::UNKNOWN);
        break;
    }
}

tmdl::ParameterValue tmdl::ParameterValue::from_string(const std::string& s, const Type t)
{
    ParameterValue data_value{};
    data_value.dtype = t;

    try
    {
        switch (data_value.dtype)
        {
        case ParameterValue::Type::BOOLEAN:
            data_value.value.tf = std::stoi(s) != 0;
            break;
        case ParameterValue::Type::INT32:
            data_value.value.i32 = std::stoi(s);
            break;
        case ParameterValue::Type::UINT32:
            data_value.value.u32 = std::stoul(s);
            break;
        case ParameterValue::Type::SINGLE:
            data_value.value.f32 = std::stof(s);
            break;
        case ParameterValue::Type::DOUBLE:
            data_value.value.f64 = std::stod(s);
            break;
        case ParameterValue::Type::DATA_TYPE:
            data_value.value.dtype = data_type_from_string(s);
            break;
        case ParameterValue::Type::UNKNOWN:
            break;
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

    return data_value;
}

std::shared_ptr<tmdl::ValueBox> tmdl::ParameterValue::to_box() const
{
    switch (dtype)
    {
    case ParameterValue::Type::BOOLEAN:
        return std::make_shared<ValueBoxType<bool>>(value.tf);
    case ParameterValue::Type::INT32:
        return std::make_shared<ValueBoxType<int32_t>>(value.i32);
    case ParameterValue::Type::UINT32:
        return std::make_shared<ValueBoxType<uint32_t>>(value.u32);
    case ParameterValue::Type::SINGLE:
        return std::make_shared<ValueBoxType<float>>(value.f32);
    case ParameterValue::Type::DOUBLE:
        return std::make_shared<ValueBoxType<double>>(value.f64);
    default:
        throw ModelException("unable to parse provided parameter into a boxed type");
    }
}

tmdl::Parameter::Parameter(
    const std::string& id,
    const std::string& name,
    const ParameterValue value) :
    id(id),
    name(name),
    value(value),
    enabled(true)
{
    // Empty Constructor
}

std::string tmdl::Parameter::get_id() const
{
    return id;
}

std::string tmdl::Parameter::get_name() const
{
    return name;
}

void tmdl::Parameter::set_name(const std::string& n)
{
    name = n;
}

bool tmdl::Parameter::get_enabled() const
{
    return enabled;
}

void tmdl::Parameter::set_enabled(const bool v)
{
    enabled = v;
}

tmdl::ParameterValue& tmdl::Parameter::get_value()
{
    return value;
}

const tmdl::ParameterValue& tmdl::Parameter::get_value() const
{
    return value;
}
