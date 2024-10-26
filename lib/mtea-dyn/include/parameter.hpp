// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNPARAMETER_HPP
#define MTEA_DYNPARAMETER_HPP

#include "data_type.hpp"
#include "value.hpp"

#include <string>

namespace mtea {

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

class ParameterIdentifier : public Parameter {
public:
    explicit ParameterIdentifier(std::string_view id, std::string_view name, const Identifier& value);

    Identifier get_value() const;
    void set_value(Identifier val);

    std::string get_value_string() const override;
    void set_value_string(std::string_view val) override;

private:
    Identifier ident;
};

}

#endif // MTEA_DYNPARAMETER_HPP
