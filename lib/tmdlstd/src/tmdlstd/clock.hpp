// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_CLOCK_H
#define TMDL_STDLIB_CLOCK_H

namespace tmdlstd
{

struct clock_block
{
    struct output_t
    {
        double val;
    };

    clock_block(const double dt);

    void reset();

    void post_step();

    output_t s_out;
    const double dt;
};

}

#endif // TMDL_STDLIB_CLOCK_H
