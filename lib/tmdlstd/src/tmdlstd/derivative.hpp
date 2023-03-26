// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_DERIVATIVE_H
#define TMDL_STDLIB_DERIVATIVE_H

namespace tmdlstd
{

template <typename T>
struct derivative_block
{
    struct input_t
    {
        const T* input_value;
        const bool* reset_flag;
    };

    struct output_t
    {
        T output_value;
    };

    derivative_block(const double dt) : dt(dt)
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

        s_out.output_value = (*s_in.input_value - last_value) / dt;
        last_value = *s_in.input_value;
    }

    void reset()
    {
        last_value = *s_in.input_value;
    }

    input_t s_in;
    output_t s_out;

    T last_value;
    const double dt;
};

}

#endif // TMDL_STDLIB_DERIVATIVE_H
