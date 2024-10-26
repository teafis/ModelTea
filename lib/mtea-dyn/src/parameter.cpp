// SPDX-License-Identifier: GPL-3.0-only

#include "parameter.hpp"

#include "model_exception.hpp"

#include <fmt/format.h>

mtea::Parameter::Parameter(std::string_view id, std::string_view name) : id(id), name(name) {}

std::string mtea::Parameter::get_id() const { return id; }

std::string mtea::Parameter::get_name() const { return name; }

void mtea::Parameter::set_name(const std::string_view n) { name = n; }

bool mtea::Parameter::get_enabled() const { return enabled; }

void mtea::Parameter::set_enabled(const bool v) { enabled = v; }

mtea::ParameterDataType::ParameterDataType(std::string_view id, std::string_view name, DataType val)
    : mtea::Parameter(id, name), value{val} {}

mtea::DataType& mtea::ParameterDataType::get_type() { return value; }
const mtea::DataType& mtea::ParameterDataType::get_type() const { return value; }

void mtea::ParameterDataType::set_type(const DataType& val) { value = val; }

std::string mtea::ParameterDataType::get_value_string() const {
    const auto dt = mtea::get_meta_type(value);
    if (dt != nullptr) {
        return dt->get_name();
    } else {
        return "none";
    }
}

void mtea::ParameterDataType::set_value_string(std::string_view val) {
    const auto dt = mtea::get_meta_type(val);
    if (dt != nullptr) {
        value = dt->get_data_type();
    } else if (val == "none") {
        value = DataType::NONE;
    } else {
        throw ModelException(fmt::format("unknown data type {} provided", val));
    }
}

mtea::ParameterValue::ParameterValue(std::string_view id, std::string_view name, std::unique_ptr<ModelValue>&& value)
    : mtea::Parameter(id, name), value(std::move(value)) {}

mtea::ModelValue* mtea::ParameterValue::get_value() { return value.get(); }

const mtea::ModelValue* mtea::ParameterValue::get_value() const { return value.get(); }

void mtea::ParameterValue::set_value(std::unique_ptr<ModelValue>&& val) { value = std::move(val); }

void mtea::ParameterValue::convert_type(const DataType dt) {
    try {
        value = ModelValue::convert_type(value.get(), dt);
    } catch (const ModelException&) {
        value = ModelValue::make_default(dt);
    }
}

std::string mtea::ParameterValue::get_value_string() const { return value->to_string(); }

void mtea::ParameterValue::set_value_string(std::string_view val) { value = ModelValue::from_string(val, value->data_type()); }

mtea::ParameterIdentifier::ParameterIdentifier(std::string_view id, std::string_view name, const Identifier& value)
    : mtea::Parameter(id, name), ident(value) {}

mtea::Identifier mtea::ParameterIdentifier::get_value() const { return ident; }
void mtea::ParameterIdentifier::set_value(Identifier val) { ident = val; }

std::string mtea::ParameterIdentifier::get_value_string() const { return ident.get(); }
void mtea::ParameterIdentifier::set_value_string(std::string_view val) { ident.set(val); }
