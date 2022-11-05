// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_SIM_MATH_SIGNAL_H
#define TF_SIM_MATH_SIGNAL_H

#include <cstdint>

#include <sim_math/data_types.hpp>

#include <string>
#include <vector>

namespace sim_math
{

struct Signal
{
    virtual DataType get_data_type() const = 0;
};

struct DoubleSignal : Signal
{
    double value = 0.0;

    DoubleSignal(const double value);

    virtual DataType get_data_type() const override;
};

struct Int32Signal : Signal
{
    int32_t value = 0;

    Int32Signal(const int32_t value);

    virtual DataType get_data_type() const override;
};

struct Int64Signal : Signal
{
    int64_t value = 0;

    Int64Signal(const int64_t value);

    virtual DataType get_data_type() const override;
};

struct BooleanSignal : Signal
{
    bool value = false;

    BooleanSignal(const bool value);

    virtual DataType get_data_type() const override;
};

struct EnumerationSignal : Signal
{
    virtual std::vector<std::string> get_options() const = 0;

    virtual bool set_from_string(const std::string& s) = 0;
};

struct DataTypeSignal : EnumerationSignal
{
    DataType value = DataType::DOUBLE;

    DataTypeSignal(const DataType value);

    virtual DataType get_data_type() const override;

    virtual std::vector<std::string> get_options() const override;

    virtual bool set_from_string(const std::string& s);
};

}

#endif // TF_SIM_MATH_SIGNAL_H
