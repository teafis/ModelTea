// SPDX-License-Identifier: GPL-3.0-only

#include "sim_state.hpp"

#include "model_exception.hpp"

#include <fmt/format.h>


tmdl::SimState::SimState(const double dt) :
    time(0.0),
    dt(dt)
{
    if (dt <= 0.0)
    {
        throw ModelException(fmt::format("unable to construct sim state with a dt of {}, as it is <= 0.0", dt));
    }
}

double tmdl::SimState::get_time() const
{
    return time;
}

double tmdl::SimState::get_dt() const
{
    return dt;
}

void tmdl::SimState::set_time(const double& t)
{
    time = t;
}
