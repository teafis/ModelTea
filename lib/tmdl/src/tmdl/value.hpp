// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VALUE_HPP
#define TF_MODEL_VALUE_HPP

#include <cstdint>

#include <memory>


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
    const void* ptr = nullptr;

    bool operator==(const PortValue& other) const = default;
};

struct ValueBox
{
    virtual ~ValueBox()
    {
        // Empty Destructor
    }

    virtual void* get_ptr_val() const = 0;
};

template <typename T>
struct ValueBoxType : public ValueBox
{
    ValueBoxType(const T inval)
    {
        value = std::make_unique<T>(inval);
    }

    virtual void* get_ptr_val() const
    {
        return value.get();
    }

    std::unique_ptr<T> value;
};

}

#endif // TF_MODEL_VALUE_HPP
