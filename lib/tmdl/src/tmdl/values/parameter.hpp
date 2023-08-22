// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_PARAMETER_HPP
#define TF_MODEL_PARAMETER_HPP

#include "data_types.hpp"
#include "value.hpp"

#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>


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
        DATA_TYPE,
        ENUM
    };

    static Type parameter_type_from_data_type(DataType dt);

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

    std::shared_ptr<ModelValue> to_box() const;

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

    void set_name(const std::string& n);

    bool get_enabled() const;

    void set_enabled(const bool v);

    ParameterValue& get_value();

    const ParameterValue& get_value() const;

protected:
    const std::string id;
    std::string name;

    ParameterValue value;

    bool enabled;
};

}

#endif // TF_MODEL_PARAMETER_HPP
