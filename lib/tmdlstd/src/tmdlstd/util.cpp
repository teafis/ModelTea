// SPDX-License-Identifier: MIT

#include "util.hpp"

const char* tmdl::stdlib::arith_to_string(const ArithType t)
{
    switch (t)
    {
    case ArithType::ADD:
        return "ADD";
    case ArithType::SUB:
        return "SUB";
    case ArithType::MUL:
        return "MUL";
    case ArithType::DIV:
        return "DIV";
    default:
        return "";
    }
}
