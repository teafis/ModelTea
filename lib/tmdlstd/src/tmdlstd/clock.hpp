// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_CLOCK_H
#define TMDL_STDLIB_CLOCK_H

namespace tmdl::stdlib
{

struct clock_block
{
    struct output_t
    {
        double val;
    };

    clock_block(const double dt);

    void reset();

    void step();

    output_t s_out;

private:
    const double dt;
};

}

#endif // TMDL_STDLIB_CLOCK_H
