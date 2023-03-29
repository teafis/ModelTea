// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_RELATIONAL_H
#define TMDL_STDLIB_RELATIONAL_H

namespace tmdl::stdlib
{

enum class RelationalOperator
{
    EQUAL = 0,
    NOT_EQUAL,
    GREATER_THAN,
    GREATER_THAN_EQUAL,
    LESS_THAN,
    LESS_THAN_EQUAL,
};

template <typename T, RelationalOperator OP>
struct relational_block
{
    struct input_t
    {
        const T* val_a;
        const T* val_b;
    };

    struct output_t
    {
        T output_value;
    };

    void step()
    {
        bool v = false;
        const T a = *s_in.val_a;
        const T b = *s_in.val_b;

        if constexpr (OP == RelationalOperator::EQUAL)
        {
            v = a == b;
        }
        else if constexpr (OP == RelationalOperator::NOT_EQUAL)
        {
            v = a != b;
        }
        else if constexpr (OP == RelationalOperator::GREATER_THAN)
        {
            v = a > b;
        }
        else if constexpr (OP == RelationalOperator::GREATER_THAN_EQUAL)
        {
            v = a >= b;
        }
        else if constexpr (OP == RelationalOperator::LESS_THAN)
        {
            v = a < b;
        }
        else if constexpr (OP == RelationalOperator::LESS_THAN_EQUAL)
        {
            v = a <= b;
        }

        s_out.output_value = v;
    }

    input_t s_in;
    output_t s_out;
};

}

#endif // TMDL_STDLIB_RELATIONAL_H
