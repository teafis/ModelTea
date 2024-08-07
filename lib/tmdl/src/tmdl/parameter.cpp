// SPDX-License-Identifier: GPL-3.0-only

module;

#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>

export module tmdl:parameter;

import :data_types;
import :value;

namespace tmdl {

export class Parameter {
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

export class ParameterDataType : public Parameter {
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

export class ParameterValue : public Parameter {
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

tmdl::Parameter::Parameter(std::string_view id, std::string_view name) : id(id), name(name) {}

std::string tmdl::Parameter::get_id() const { return id; }

std::string tmdl::Parameter::get_name() const { return name; }

void tmdl::Parameter::set_name(const std::string_view n) { name = n; }

bool tmdl::Parameter::get_enabled() const { return enabled; }

void tmdl::Parameter::set_enabled(const bool v) { enabled = v; }

tmdl::ParameterDataType::ParameterDataType(std::string_view id, std::string_view name, DataType val)
    : tmdl::Parameter(id, name), value{val} {}

tmdl::DataType& tmdl::ParameterDataType::get_type() { return value; }
const tmdl::DataType& tmdl::ParameterDataType::get_type() const { return value; }

void tmdl::ParameterDataType::set_type(const DataType& val) { value = val; }

std::string tmdl::ParameterDataType::get_value_string() const {
    const auto dt = mt::stdlib::get_meta_type(value);
    if (dt != nullptr) {
        return dt->get_name();
    } else {
        return "none";
    }
}

void tmdl::ParameterDataType::set_value_string(std::string_view val) {
    const auto dt = mt::stdlib::get_meta_type(val);
    if (dt != nullptr) {
        value = dt->get_data_type();
    } else if (val == "none") {
        value = DataType::NONE;
    } else {
            throw ModelException(fmt::format("unknown data type {} provided", val));
    }
}

tmdl::ParameterValue::ParameterValue(std::string_view id, std::string_view name, std::unique_ptr<ModelValue>&& value)
    : tmdl::Parameter(id, name), value(std::move(value)) {}

tmdl::ModelValue* tmdl::ParameterValue::get_value() { return value.get(); }

const tmdl::ModelValue* tmdl::ParameterValue::get_value() const { return value.get(); }

void tmdl::ParameterValue::set_value(std::unique_ptr<ModelValue>&& val) { value = std::move(val); }

void tmdl::ParameterValue::convert_type(const DataType dt) {
    try {
        value = ModelValue::convert_type(value.get(), dt);
    } catch (const ModelException&) {
        value = ModelValue::make_default(dt);
    }
}

std::string tmdl::ParameterValue::get_value_string() const { return value->to_string(); }

void tmdl::ParameterValue::set_value_string(std::string_view val) { value = ModelValue::from_string(val, value->data_type()); }
