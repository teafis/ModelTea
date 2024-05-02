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
    Parameter(std::string_view id, std::string_view name);
    virtual ~Parameter() = default;

    virtual std::string get_id() const;
    virtual std::string get_name() const;
    virtual void set_name(std::string_view n);
    virtual bool get_enabled() const;
    virtual void set_enabled(const bool v) final;

    virtual std::string get_value_string() const = 0;
    virtual void set_value_string(std::string_view val) = 0;

private:
    const std::string id;
    std::string name;
    bool enabled{true};
};

class ParameterDataType : public Parameter {
public:
    ParameterDataType(std::string_view id, std::string_view name, DataType val = DataType::NONE);

    DataType& get_type();
    const DataType& get_type() const;
    void set_type(const DataType& val);

    std::string get_value_string() const override;
    void set_value_string(std::string_view val) override;

private:
    DataType value;
};

class ParameterValue : public Parameter {
public:
    explicit ParameterValue(std::string_view id, std::string_view name, std::unique_ptr<ModelValue>&& value);

    ModelValue* get_value();
    const ModelValue* get_value() const;
    void set_value(std::unique_ptr<ModelValue>&& val);

    void convert_type(const DataType dt);

    std::string get_value_string() const override;
    void set_value_string(std::string_view val) override;

private:
    std::unique_ptr<ModelValue> value;
};
}

#endif // TF_MODEL_PARAMETER_HPP
