//SPDX-License-Identifier: MIT

#include "tmdlstd.hpp"

tmdl::stdlib::clock_block::clock_block(const double dt) : dt(dt)
{
    reset();
}

void tmdl::stdlib::clock_block::reset()
{
    s_out.val = 0.0;
}

void tmdl::stdlib::clock_block::step()
{
    s_out.val += dt;
}

const char* tmdl::stdlib::arith_to_string(const ArithType t)
{
    switch (t)
    {
    case ArithType::ADD:
        return "ArithType::ADD";
    case ArithType::SUB:
        return "ArithType::SUB";
    case ArithType::MUL:
        return "ArithType::MUL";
    case ArithType::DIV:
        return "ArithType::DIV";
    default:
        return "";
    }
}

const char* tmdl::stdlib::relational_to_string(const RelationalOperator op)
{
    switch (op)
    {
    case RelationalOperator::EQUAL:
        return "RelationalOperator::EQUAL";
    case RelationalOperator::NOT_EQUAL:
        return "RelationalOperator::NOT_EQUAL";
    case RelationalOperator::GREATER_THAN:
        return "RelationalOperator::GREATER_THAN";
    case RelationalOperator::GREATER_THAN_EQUAL:
        return "RelationalOperator::GREATER_THAN_EQUAL";
    case RelationalOperator::LESS_THAN:
        return "RelationalOperator::LESS_THAN";
    case RelationalOperator::LESS_THAN_EQUAL:
        return "RelationalOperator::LESS_THAN_EQUAL";
    default:
        return "";
    }
}

const char* tmdl::stdlib::trig_func_to_string(const TrigFunction fcn)
{
    switch (fcn)
    {
    case TrigFunction::SIN:
        return "TrigFunction::SIN";
    case TrigFunction::COS:
        return "TrigFunction::COS";
    default:
        return "";
    }
}
