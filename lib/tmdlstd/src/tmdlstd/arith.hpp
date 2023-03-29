// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_ARITH_H
#define TMDL_STDLIB_ARITH_H

#include <array>

namespace tmdlstd
{

enum class ArithType
{
    ADD = 0,
    SUB,
    MUL,
    DIV
};

template <typename T, ArithType AT>
struct arith_block_dynamic
{
    struct input_t
    {
        const T** vals;
        int size;
    };

    struct output_t
    {
        T val;
    };

    void step()
    {
        T val = *s_in.vals[0];

        for (int i = 1; i < s_in.size; ++i)
        {
            const auto& v = *s_in.vals[i];

            if constexpr (AT == ArithType::ADD)
            {
                val += v;
            }
            else if constexpr (AT == ArithType::SUB)
            {
                val -= v;
            }
            else if constexpr (AT == ArithType::MUL)
            {
                val *= v;
            }
            else if constexpr (AT == ArithType::DIV)
            {
                val /= v;
            }
            else
            {
                // Error...
            }
        }

        s_out.val = val;
    }

    input_t s_in;
    output_t s_out;
};

template <typename T, ArithType AT, int SIZE>
struct arith_block : public arith_block_dynamic<T, AT>
{
    arith_block()
    {
        this->s_in.size = SIZE;
        this->s_in.vals = _input_array.data();
    }

private:
    std::array<const T*, SIZE> _input_array;
};

}

#endif // TMDL_STDLIB_ARITH_H
