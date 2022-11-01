// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_SIM_MATH_SIGNAL_H
#define TF_SIM_MATH_SIGNAL_H

#include <cstdint>

#include <sim_math/data_types.hpp>

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

}

#endif // TF_SIM_MATH_SIGNAL_H
