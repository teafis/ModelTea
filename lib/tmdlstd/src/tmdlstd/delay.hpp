// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_DELAY_H
#define TMDL_STDLIB_DELAY_H

namespace tmdl::stdlib
{

template <typename T>
struct delay_block
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

        s_out.output_value = next_value;
        next_value = *s_in.input_value;
    }

    void reset()
    {
        next_value = *s_in.reset_value;
    }

    input_t s_in;
    output_t s_out;

    T next_value;
};

}

#endif // TMDL_STDLIB_DELAY_H
