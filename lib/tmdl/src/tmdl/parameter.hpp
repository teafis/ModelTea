// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_PARAMETER_HPP
#define TF_MODEL_PARAMETER_HPP

#include <string>

#include <tmdl/model_exception.hpp>
#include <tmdl/value.hpp>


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

    union {
        bool tf;
        float f32;
        double f64;
        int32_t i32;
        uint32_t u32;
        DataType dtype;
    } value{};

    Type dtype = Type::UNKNOWN;
};

class Parameter
{    
public:
    Parameter(
        const std::string& id,
        const std::string& name,
        const ParameterValue::Type data_type,
        const std::string& value) :
        id(id),
        name(name),
        value(value),
        data_type(data_type)
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

    ParameterValue::Type get_data_type() const
    {
        return data_type;
    }

    void set_data_type(const ParameterValue::Type new_type)
    {
        data_type = new_type;
    }

    void set_value(const std::string& new_value)
    {
        get_value_for_string(new_value);
        value = new_value;
    }

    ParameterValue get_current_value() const
    {
        return get_value_for_string(value);
    }

    std::string get_current_value_string() const
    {
        return value;
    }

protected:
    ParameterValue get_value_for_string(const std::string& s) const
    {
        ParameterValue data_value{};

        try
        {
            switch (data_type)
            {
            case ParameterValue::Type::BOOLEAN:
                data_value.dtype = ParameterValue::Type::BOOLEAN;
                data_value.value.tf = std::stoi(s) != 0;
                break;
            case ParameterValue::Type::INT32:
                data_value.dtype = ParameterValue::Type::INT32;
                data_value.value.i32 = std::stoi(s);
                break;
            case ParameterValue::Type::UINT32:
                data_value.dtype = ParameterValue::Type::UINT32;
                data_value.value.u32 = std::stoul(s);
                break;
            case ParameterValue::Type::SINGLE:
                data_value.dtype = ParameterValue::Type::SINGLE;
                data_value.value.f32 = std::stof(s);
                break;
            case ParameterValue::Type::DOUBLE:
                data_value.dtype = ParameterValue::Type::DOUBLE;
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

protected:
    const std::string id;
    const std::string name;

    std::string value;
    ParameterValue::Type data_type;
};

}

#endif // TF_MODEL_PARAMETER_HPP
