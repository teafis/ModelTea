// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_UTIL_H
#define TMDL_STDLIB_UTIL_H

#include "arith.hpp"
#include "relational.hpp"
#include "trig.hpp"

namespace tmdl::stdlib
{

const char* arith_to_string(ArithType t);

const char* relational_to_string(RelationalOperator op);

const char* trig_func_to_string(TrigFunction fcn);

}

#endif // TMDL_STDLIB_UTIL_H
