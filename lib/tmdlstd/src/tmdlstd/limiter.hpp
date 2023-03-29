// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_LIMITER_H
#define TMDL_STDLIB_LIMITER_H

namespace tmdl::stdlib
{

template <typename T>
struct limiter_block
{
    struct input_t
    {
        const T* input_value;
        const T* limit_upper;
        const T* limit_lower;
    };

    struct output_t
    {
        T output_value;
    };

    void step()
    {
        T x = *s_in.input_value;
        const T lu = *s_in.limit_upper;
        const T ll = *s_in.limit_lower;

        if (x < ll)
        {
            x = ll;
        }
        else if (x > lu)
        {
            x = lu;
        }

        s_out.output_value = x;
    }

    input_t s_in;
    output_t s_out;
};

}

#endif // TMDL_STDLIB_LIMITER_H
