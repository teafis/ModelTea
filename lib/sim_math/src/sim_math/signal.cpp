// SPDX-License-Identifier: GPL-3.0-only

#include <sim_math/signal.hpp>

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
