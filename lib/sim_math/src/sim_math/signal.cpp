// SPDX-License-Identifier: GPL-3.0-only

#include <sim_math/signal.hpp>

#include <optional>


using namespace sim_math;

DoubleSignal::DoubleSignal(const double value) : value(value)
{
    // Empty Constructor;
}

DataType DoubleSignal::get_data_type() const
{
    return DataType::DOUBLE;
}

Int32Signal::Int32Signal(const int32_t value) : value(value)
{
    // Empty Constructor
}

DataType Int32Signal::get_data_type() const
{
    return DataType::INT32;
}

Int64Signal::Int64Signal(const int64_t value) : value(value)
{
    // Empty Constructor
}

DataType Int64Signal::get_data_type() const
{
    return DataType::INT64;
}

BooleanSignal::BooleanSignal(const bool val) : value(val)
{
    // Empty Constructor
}

DataType BooleanSignal::get_data_type() const
{
    return DataType::BOOLEAN;
}

DataTypeSignal::DataTypeSignal(const DataType value) : value(value)
{
    // Empty Constructor
}

DataType DataTypeSignal::get_data_type() const
{
    return DataType::DATA_TYPE;
}

std::vector<std::string> DataTypeSignal::get_options() const
{
    return {
        "DOUBLE"
        "BOOLEAN"
        "INT32"
        "INT64"
    };
}

bool DataTypeSignal::set_from_string(const std::string& s)
{
    std::optional<DataType> d;

    if (s == "DOUBLE") d = DataType::DOUBLE;
    else if (s == "BOOLEAN") d = DataType::BOOLEAN;
    else if (s == "INT32") d = DataType::INT32;
    else if (s == "INT64") d = DataType::INT64;

    if (d.has_value())
    {
        value = d.value();
        return true;
    }
    else
    {
        return false;
    }
}
