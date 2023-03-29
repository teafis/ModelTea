// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_INTEGRATOR_H
#define TMDL_STDLIB_INTEGRATOR_H

namespace tmdl::stdlib
{

template <typename T>
struct integrator_block
{
    struct input_t
    {
        const T* input_value;
        const T* reset_value;
        const bool* reset_flag;
    };

    struct output_t
    {
        T output_value;
    };

    integrator_block(const double dt) : dt(dt)
    {
        // Empty Constructor
    }

    void init()
    {
        reset();
    }

    void step()
    {
        if (*s_in.reset_flag)
        {
            reset();
        }
        else
        {
            s_out.output_value += *s_in.input_value * dt;
        }
    }

    void reset()
    {
        s_out.output_value = *s_in.reset_value;
    }

    input_t s_in;
    output_t s_out;

    const double dt;
};

}

#endif // TMDL_STDLIB_INTEGRATOR_H
