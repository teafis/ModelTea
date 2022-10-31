#ifndef TF_SIM_MATH_SIGNAL_H
#define TF_SIM_MATH_SIGNAL_H

#include <cstdint>

#include "sim_types.hpp"

namespace sim_math
{

struct Signal
{
    virtual DataType get_data_type() const = 0;
};

struct DoubleSignal : Signal
{
    virtual DataType get_data_type() const override
    {
        return DataType::DOUBLE;
    }

    double value = 0.0;
};

struct Int32Signal : Signal
{
    virtual DataType get_data_type() const override
    {
        return DataType::INT32;
    }

    int32_t value = 0;
};

struct Int64Signal : Signal
{
    virtual DataType get_data_type() const override
    {
        return DataType::INT64;
    }

    int64_t value = 0;
};

struct BooleanSignal : Signal
{
    virtual DataType get_data_type() const override
    {
        return DataType::BOOLEAN;
    }

    bool value = 0.0;
};

}

#endif // TF_SIM_MATH_SIGNAL_H
