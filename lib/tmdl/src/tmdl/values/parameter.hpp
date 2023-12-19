// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_PARAMETER_HPP
#define TF_MODEL_PARAMETER_HPP

#include "data_types.hpp"
#include "value.hpp"

#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>

namespace tmdl {

class Parameter {
public:
    explicit Parameter(std::string_view id, std::string_view name, std::unique_ptr<ModelValue>&& value);

    std::string get_id() const;

    std::string get_name() const;

    void set_name(std::string_view n);

    bool get_enabled() const;

    void set_enabled(const bool v);

    ModelValue* get_value();

    const ModelValue* get_value() const;

    void set_value(std::unique_ptr<ModelValue>&& val);

    void convert_type(const DataType dt);

private:
    const std::string id;
    std::string name;

    std::unique_ptr<ModelValue> value;

    bool enabled{true};
};

}

#endif // TF_MODEL_PARAMETER_HPP
