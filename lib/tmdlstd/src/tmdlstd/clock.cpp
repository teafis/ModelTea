// SPDX-License-Identifier: MIT

#include "clock.hpp"

tmdlstd::clock_block::clock_block(const double dt) : dt(dt)
{
    reset();
}

void tmdlstd::clock_block::reset()
{
    s_out.val = 0.0;
}

void tmdlstd::clock_block::step()
{
    s_out.val += dt;
}
