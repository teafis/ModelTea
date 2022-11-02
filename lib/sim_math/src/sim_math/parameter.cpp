// SPDX-License-Identifier: GPL-3.0-only

#include <sim_math/parameter.hpp>

using namespace sim_math;

Parameter::Parameter(
    const std::string& param_id,
    const std::string& name,
    const std::string& description,
    const DataType& data_type,
    const std::string& default_value) :
    identifier(param_id),
    name(name),
    description(description),
    value(default_value),
    data_type(data_type)
{
    // Empty Constructor
}

const std::string& Parameter::get_id() const
{
    return identifier;
}

const std::string& Parameter::get_name() const
{
    return name;
}

const std::string& Parameter::get_description() const
{
    return description;
}

bool Parameter::set_value(const std::string& new_value)
{
    if (parse_parameter(new_value) != nullptr)
    {
        value = new_value;
        return true;
    }
    else
    {
        return false;
    }
}

std::shared_ptr<Signal> Parameter::get_parameter_value() const
{
    return parse_parameter(value);
}

std::shared_ptr<Signal> Parameter::parse_parameter(const std::string& s) const
{
    try
    {
        switch (data_type)
        {
        case DataType::BOOLEAN:
            return std::make_shared<BooleanSignal>(std::stoi(s) != 0);
        case DataType::INT32:
            return std::make_shared<Int32Signal>(std::stoi(s));
        case DataType::INT64:
            return std::make_shared<Int64Signal>(std::stol(s));
        case DataType::DOUBLE:
            return std::make_shared<DoubleSignal>(std::stod(s));
        default:
            return nullptr;
        }
    }
    catch (const std::invalid_argument&)
    {
        return nullptr;
    }
    catch (const std::out_of_range&)
    {
        return nullptr;
    }
}
