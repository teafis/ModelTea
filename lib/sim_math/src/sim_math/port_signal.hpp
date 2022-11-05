// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_SIM_MATH_PORT_SIGNAL_HPP
#define TF_SIM_MATH_PORT_SIGNAL_HPP

#include <sim_math/signal.hpp>

#include <memory>
#include <string>

namespace sim_math
{

class PortSignal
{
public:
    PortSignal(const std::string& name) : name(name)
    {
        // Empty Constructor
    }

    const std::string& get_name() const
    {
        return name;
    }

    std::weak_ptr<Signal> get_signal() const
    {
        return signal;
    }

    void set_signal(std::weak_ptr<Signal> sig)
    {
        signal = sig;
    }

protected:
    std::weak_ptr<Signal> signal;
    std::string name;
};

}

#endif // TF_SIM_MATH_PORT_SIGNAL_HPP
