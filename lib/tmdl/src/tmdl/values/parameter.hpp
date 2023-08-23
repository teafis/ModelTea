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

class Parameter
{    
public:
    Parameter(
        const std::string& id,
        const std::string& name,
        std::unique_ptr<ModelValue>&& value);

    std::string get_id() const;

    std::string get_name() const;

    void set_name(const std::string& n);

    bool get_enabled() const;

    void set_enabled(const bool v);

    ModelValue* get_value();

    const ModelValue* get_value() const;

    void set_value(std::unique_ptr<ModelValue>&& val);

    void convert_type(const DataType dt);

protected:
    const std::string id;
    std::string name;

    std::unique_ptr<ModelValue> value;

    bool enabled;
};

}

#endif // TF_MODEL_PARAMETER_HPP
