// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/value.hpp>
#include <tmdl/model_exception.hpp>

using namespace tmdl;


/* ========== DOUBLE VALUE ========== */

DoubleValue::DoubleValue(const double value) : value(value)
{
    // Empty Constructor
}

DataType DoubleValue::get_data_type() const
{
    return DataType::DOUBLE;
}

std::unique_ptr<Value> DoubleValue::clone() const
{
    return std::make_unique<DoubleValue>(*this);
}

void DoubleValue::copy_from(const Value* other)
{
    const DoubleValue* dst = dynamic_cast<const DoubleValue*>(other);
    if (dst == nullptr)
    {
        throw ModelException("cannot copy from provided value");
    }
    value = dst->value;
}

/* ========== BOOLEAN VALUE ========== */

BooleanValue::BooleanValue(const bool value) : value(value)
{
    // Empty Constructor
}

DataType BooleanValue::get_data_type() const
{
    return DataType::BOOLEAN;
}

std::unique_ptr<Value> BooleanValue::clone() const
{
    return std::make_unique<BooleanValue>(*this);
}

void BooleanValue::copy_from(const Value* other)
{
    const BooleanValue* dst = dynamic_cast<const BooleanValue*>(other);
    if (dst == nullptr)
    {
        throw ModelException("cannot copy from provided value");
    }
    value = dst->value;
}

/* ========== UINT32 VALUE ========== */

UInt32Value::UInt32Value(const uint32_t value) : value(value)
{
    // Empty Constructor
}

DataType UInt32Value::get_data_type() const
{
    return DataType::UINT32;
}

std::unique_ptr<Value> UInt32Value::clone() const
{
    return std::make_unique<UInt32Value>(value);
}

void UInt32Value::copy_from(const Value* other)
{
    const UInt32Value* dst = dynamic_cast<const UInt32Value*>(other);
    if (dst == nullptr)
    {
        throw ModelException("cannot copy from provided value");
    }
    value = dst->value;
}

/* ========== INT32 VALUE ========== */

Int32Value::Int32Value(const int32_t value) : value(value)
{
    // Empty Constructor
}

DataType Int32Value::get_data_type() const
{
    return DataType::INT32;
}

std::unique_ptr<Value> Int32Value::clone() const
{
    return std::make_unique<Int32Value>(*this);
}

void Int32Value::copy_from(const Value* other)
{
    const Int32Value* dst = dynamic_cast<const Int32Value*>(other);
    if (dst == nullptr)
    {
        throw ModelException("cannot copy from provided value");
    }
    value = dst->value;
}

/* ========== UTILITY VALUES ========== */

std::unique_ptr<Value> make_value_for_type(const DataType dt)
{
    switch (dt)
    {
    case DataType::DOUBLE:
        return std::make_unique<DoubleValue>();
    case DataType::INT32:
        return std::make_unique<Int32Value>();
    case DataType::UINT32:
        return std::make_unique<UInt32Value>();
    default:
        return nullptr;
    }
}
