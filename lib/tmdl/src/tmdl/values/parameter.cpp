// SPDX-License-Identifier: GPL-3.0-only

#include "parameter.hpp"

#include "../model_exception.hpp"

#include <iomanip>

#include <fmt/format.h>


tmdl::Parameter::Parameter(
    const std::string& id,
    const std::string& name,
    std::unique_ptr<ModelValue>&& value) :
    id(id),
    name(name),
    value(std::move(value)),
    enabled(true)
{
    // Empty Constructor
}

std::string tmdl::Parameter::get_id() const
{
    return id;
}

std::string tmdl::Parameter::get_name() const
{
    return name;
}

void tmdl::Parameter::set_name(const std::string& n)
{
    name = n;
}

bool tmdl::Parameter::get_enabled() const
{
    return enabled;
}

void tmdl::Parameter::set_enabled(const bool v)
{
    enabled = v;
}

tmdl::ModelValue* tmdl::Parameter::get_value()
{
    return value.get();
}

const tmdl::ModelValue* tmdl::Parameter::get_value() const
{
    return value.get();
}

void tmdl::Parameter::set_value(std::unique_ptr<ModelValue>&& val)
{
    value = std::move(val);
}

void tmdl::Parameter::convert_type(const DataType dt)
{
    try
    {
        value = ModelValue::convert_type(value.get(), dt);
    }
    catch (const ModelException&)
    {
        value = ModelValue::make_default(dt);
    }
}
