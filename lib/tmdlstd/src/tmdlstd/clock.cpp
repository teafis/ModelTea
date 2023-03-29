// SPDX-License-Identifier: MIT

#include "clock.hpp"

tmdl::stdlib::clock_block::clock_block(const double dt) : dt(dt)
{
    reset();
}

void tmdl::stdlib::clock_block::reset()
{
    s_out.val = 0.0;
}

void tmdl::stdlib::clock_block::step()
{
    s_out.val += dt;
}
