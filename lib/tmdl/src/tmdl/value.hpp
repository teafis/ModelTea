// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VALUE_HPP
#define TF_MODEL_VALUE_HPP

#include <cstdint>

#include <string>
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

template<DataType>
struct data_type_t
{
    static const bool has_value = false;
    static const bool is_numeric = false;
};

template<>
struct data_type_t<DataType::SINGLE>
{
    using value = float;
    static const bool has_value = true;
    static const bool is_numeric = true;
};

template<>
struct data_type_t<DataType::DOUBLE>
{
    using value = double;
    static const bool has_value = true;
    static const bool is_numeric = true;
};

template<>
struct data_type_t<DataType::INT32>
{
    using value = int32_t;
    static const bool has_value = true;
    static const bool is_numeric = true;
};

template<>
struct data_type_t<DataType::UINT32>
{
    using value = uint32_t;
    static const bool has_value = true;
    static const bool is_numeric = true;
};

template<>
struct data_type_t<DataType::BOOLEAN>
{
    using value = bool;
    static const bool has_value = true;
    static const bool is_numeric = false;
};

std::string data_type_to_string(const DataType dtype);

DataType data_type_from_string(const std::string& s);

struct PortValue
{
    DataType dtype = DataType::UNKNOWN;
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
