// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_PARAMETER_HPP
#define TF_MODEL_PARAMETER_HPP

#include <string>

#include <tmdl/model_exception.hpp>
#include <tmdl/value.hpp>

#include <sstream>
#include <iomanip>


namespace tmdl
{

struct ParameterValue
{    
    enum class Type
    {
        UNKNOWN = 0,
        BOOLEAN,
        SINGLE,
        DOUBLE,
        INT32,
        UINT32,
        DATA_TYPE
    };

    union Value {
        bool tf;
        float f32;
        double f64;
        int32_t i32;
        uint32_t u32;
        DataType dtype;
    };

    Type dtype = Type::UNKNOWN;

    Value value{};

    std::string to_string() const
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
        default:
            throw ModelException("unsupported type");
        }

        return oss.str();
    }

    void convert(const Type t)
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

    static ParameterValue from_string(const std::string& s, const Type t)
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
            {
                throw ModelException("not implemented");
            }
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
};

class Parameter
{    
public:
    Parameter(
        const std::string& id,
        const std::string& name,
        const ParameterValue value) :
        id(id),
        name(name),
        value(value)
    {
        // Empty Constructor
    }

    std::string get_id() const
    {
        return id;
    }

    std::string get_name() const
    {
        return name;
    }

    ParameterValue& get_value()
    {
        return value;
    }

    const ParameterValue& get_value() const
    {
        return value;
    }

protected:
    const std::string id;
    const std::string name;

    ParameterValue value;
};

}

#endif // TF_MODEL_PARAMETER_HPP
