// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VALUE_HPP
#define TF_MODEL_VALUE_HPP

#include <cstdint>

#include <memory>
#include <optional>


namespace tmdl
{

enum class DataType
{
    UNKNOWN,
    SINGLE,
    DOUBLE,
    BOOLEAN,
    INT32,
    UINT32
};

struct PortValue
{
    DataType dtype = DataType::UNKNOWN;

    bool is_delayed_output = false;

    PortValue& operator=(const PortValue&) = delete;
};

struct ValueBox
{
    virtual ~ValueBox()
    {
        // Empty Destructor
    }
};

template <typename T>
struct ValueBoxType : public ValueBox
{
    ValueBoxType(const T inval)
    {
        value = inval;
    }

    T value;
};

std::shared_ptr<ValueBox> make_shared_default_value(const DataType dtype);

}

#endif // TF_MODEL_VALUE_HPP
