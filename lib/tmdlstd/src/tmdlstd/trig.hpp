// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_TRIG_H
#define TMDL_STDLIB_TRIG_H

namespace tmdl::stdlib
{

enum class TrigFunction
{
    SIN = 0,
    COS
};

template <typename T, TrigFunction FCN>
struct trig_block
{
    struct input_t
    {
        const T* value;
    };

    struct output_t
    {
        T value;
    };

    void step()
    {
        T y{};
        const T x = *s_in.value;

        if constexpr (FCN == TrigFunction::SIN)
        {
            y = std::sin(x);
        }
        else if constexpr (FCN == TrigFunction::COS)
        {
            y = std::cos(x);
        }

        s_out.value = y;
    }

    input_t s_in;
    output_t s_out;
};

}

#endif // TMDL_STDLIB_TRIG_H
