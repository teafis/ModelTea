#ifndef TF_SIM_MATH_BUS_H
#define TF_SIM_MATH_BUS_H

#include <unordered_map>
#include <string>
#include <memory>

#include <sim_math/signal.hpp>

namespace sim_math
{

struct BusSignal : Signal
{
    virtual DataType get_data_type() const override
    {
        return DataType::BUS;
    }

    // value - TODO
};

struct Bus
{
    std::unordered_map<std::string, std::shared_ptr<Signal>> signals;
};

}

#endif // TF_SIM_MATH_BUS_H
