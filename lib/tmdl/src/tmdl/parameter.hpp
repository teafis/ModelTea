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

    std::string to_string() const;

    void convert(const Type t);

    static ParameterValue from_string(const std::string& s, const Type t);
};

class Parameter
{    
public:
    Parameter(
        const std::string& id,
        const std::string& name,
        const ParameterValue value);

    std::string get_id() const;

    std::string get_name() const;

    ParameterValue& get_value();

    const ParameterValue& get_value() const;

protected:
    const std::string id;
    const std::string name;

    ParameterValue value;
};

}

#endif // TF_MODEL_PARAMETER_HPP
