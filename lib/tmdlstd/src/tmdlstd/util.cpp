// SPDX-License-Identifier: MIT

#include "util.hpp"

consteval const char* tmdl::stdlib::arith_to_string(const ArithType t)
{
    if (t == ArithType::ADD)
    {
        return "ADD";
    }
    else if (t == ArithType::SUB)
    {
        return "SUB";
    }
    else if (t == ArithType::MUL)
    {
        return "MUL";
    }
    else if (t == ArithType::DIV)
    {
        return "DIV";
    }
    else
    {
        static_assert("false");
        return "";
    }
}
