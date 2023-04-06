//SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_H
#define TMDL_STDLIB_H

#include <array>
#include <cmath>

namespace tmdl::stdlib
{

enum class ArithType
{
    ADD = 0,
    SUB,
    MUL,
    DIV
};

const char* arith_to_string(ArithType t);

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

template <typename T>
struct const_block
{
    struct output_t
    {
        const T val;
    };

    const_block(const T val) : s_out{ .val = val }
    {
        // Empty Constructor
    }

    const output_t s_out;
};

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

enum class RelationalOperator
{
    EQUAL = 0,
    NOT_EQUAL,
    GREATER_THAN,
    GREATER_THAN_EQUAL,
    LESS_THAN,
    LESS_THAN_EQUAL,
};

const char* relational_to_string(RelationalOperator op);

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

enum class TrigFunction
{
    SIN = 0,
    COS
};

const char* trig_func_to_string(TrigFunction fcn);

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

#endif // TMDL_STDLIB_H
