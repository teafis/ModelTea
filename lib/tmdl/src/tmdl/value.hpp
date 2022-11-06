// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VALUE_HPP
#define TF_MODEL_VALUE_HPP

#include <cstdint>

#include <memory>


namespace tmdl
{

enum class DataType
{
    DATA_TYPE,
    DOUBLE,
    BOOLEAN,
    INT32,
    UINT32
};

class Value
{
public:
    virtual DataType get_data_type() const = 0;

    virtual std::unique_ptr<Value> clone() const = 0;
};

class DoubleValue : public Value
{
public:
    DoubleValue(const double value = 0.0);

    DataType get_data_type() const override;

    std::unique_ptr<Value> clone() const override;

public:
    double value;
};

class BooleanValue : public Value
{
public:
    BooleanValue(const bool value = false);

    DataType get_data_type() const override;

    std::unique_ptr<Value> clone() const override;

public:
    bool value;
};

class UInt32Value : public Value
{
public:
    UInt32Value(const uint32_t value = 0.0);

    DataType get_data_type() const override;

    std::unique_ptr<Value> clone() const override;

public:
    uint32_t value;
};

class Int32Value : public Value
{
public:
    Int32Value(const int32_t value = 0.0);

    DataType get_data_type() const override;

    std::unique_ptr<Value> clone() const override;

public:
    int32_t value;
};

std::unique_ptr<Value> make_value_for_type(const DataType dt);

}

#endif // TF_MODEL_VALUE_HPP
