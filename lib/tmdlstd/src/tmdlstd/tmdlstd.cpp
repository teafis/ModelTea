//SPDX-License-Identifier: MIT

#include "tmdlstd.hpp"

tmdl::stdlib::clock_block::clock_block(const double dt) : dt{ dt }
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
        return "tmdl::stdlib::ArithType::ADD";
    case ArithType::SUB:
        return "tmdl::stdlib::ArithType::SUB";
    case ArithType::MUL:
        return "tmdl::stdlib::ArithType::MUL";
    case ArithType::DIV:
        return "tmdl::stdlib::ArithType::DIV";
    default:
        return "";
    }
}

const char* tmdl::stdlib::relational_to_string(const RelationalOperator op)
{
    switch (op)
    {
    case RelationalOperator::EQUAL:
        return "tmdl::stdlib::RelationalOperator::EQUAL";
    case RelationalOperator::NOT_EQUAL:
        return "tmdl::stdlib::RelationalOperator::NOT_EQUAL";
    case RelationalOperator::GREATER_THAN:
        return "tmdl::stdlib::RelationalOperator::GREATER_THAN";
    case RelationalOperator::GREATER_THAN_EQUAL:
        return "tmdl::stdlib::RelationalOperator::GREATER_THAN_EQUAL";
    case RelationalOperator::LESS_THAN:
        return "tmdl::stdlib::RelationalOperator::LESS_THAN";
    case RelationalOperator::LESS_THAN_EQUAL:
        return "tmdl::stdlib::RelationalOperator::LESS_THAN_EQUAL";
    default:
        return "";
    }
}

const char* tmdl::stdlib::trig_func_to_string(const TrigFunction fcn)
{
    switch (fcn)
    {
    case TrigFunction::SIN:
        return "tmdl::stdlib::TrigFunction::SIN";
    case TrigFunction::COS:
        return "tmdl::stdlib::TrigFunction::COS";
    default:
        return "";
    }
}
