// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_DATA_TYPES_H
#define TF_MODEL_DATA_TYPES_H

#include <cstdint>
#include <string>

namespace tmdl
{

enum class DataType : uint32_t
{
    UNKNOWN = 0,
    SINGLE,
    DOUBLE,
    BOOLEAN,
    INT32,
    UINT32,
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
    using type = float;
    static const bool has_value = true;
    static const bool is_numeric = true;
};

template<>
struct data_type_t<DataType::DOUBLE>
{
    using type = double;
    static const bool has_value = true;
    static const bool is_numeric = true;
};

template<>
struct data_type_t<DataType::INT32>
{
    using type = int32_t;
    static const bool has_value = true;
    static const bool is_numeric = true;
};

template<>
struct data_type_t<DataType::UINT32>
{
    using type = uint32_t;
    static const bool has_value = true;
    static const bool is_numeric = true;
};

template<>
struct data_type_t<DataType::BOOLEAN>
{
    using type = bool;
    static const bool has_value = true;
    static const bool is_numeric = false;
};

std::string data_type_to_string(const DataType dtype);

DataType data_type_from_string(const std::string& s);

}

#endif // TF_MODEL_DATA_TYPES_H
