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

    clock_block(const double dt) : dt(dt)
    {
        reset();
    }

    void reset()
    {
        s_out.val = 0.0;
    }

    void post_step()
    {
        s_out.val += dt;
    }

    output_t s_out;
    const double dt;
};

}

#endif // TMDL_STDLIB_CLOCK_H
