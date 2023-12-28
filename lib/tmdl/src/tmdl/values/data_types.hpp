// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_DATA_TYPES_H
#define TF_MODEL_DATA_TYPES_H

#include <cstdint>
#include <string>

#include "identifiers.hpp"

#include "mtstd_types.hpp"

namespace tmdl {

enum class DataType : uint32_t {
    UNKNOWN = 0,
    SINGLE,
    DOUBLE,
    BOOLEAN,
    INT32,
    UINT32,
    DATA_TYPE,
    IDENTIFIER,
};

template <DataType> struct data_type_t {
    static const bool has_value = false;
    static const bool is_numeric = false;
    static const bool is_modelable = false;
};

template <> struct data_type_t<DataType::SINGLE> {
    static const bool has_value = true;
    static const bool is_numeric = true;
    static const bool is_modelable = true;
    static const mt::stdlib::DataType mt_data_type = mt::stdlib::DataType::F32;
    using type = mt::stdlib::type_info<mt_data_type>::type_t;
};

template <> struct data_type_t<DataType::DOUBLE> {
    static const bool has_value = true;
    static const bool is_numeric = true;
    static const bool is_modelable = true;
    static const mt::stdlib::DataType mt_data_type = mt::stdlib::DataType::F64;
    using type = mt::stdlib::type_info<mt_data_type>::type_t;
};

template <> struct data_type_t<DataType::INT32> {
    static const bool has_value = true;
    static const bool is_numeric = true;
    static const bool is_modelable = true;
    static const mt::stdlib::DataType mt_data_type = mt::stdlib::DataType::I32;
    using type = mt::stdlib::type_info<mt_data_type>::type_t;
};

template <> struct data_type_t<DataType::UINT32> {
    static const bool has_value = true;
    static const bool is_numeric = true;
    static const bool is_modelable = true;
    static const mt::stdlib::DataType mt_data_type = mt::stdlib::DataType::U32;
    using type = mt::stdlib::type_info<mt_data_type>::type_t;
};

template <> struct data_type_t<DataType::BOOLEAN> {
    static const bool has_value = true;
    static const bool is_numeric = false;
    static const bool is_modelable = true;
    static const mt::stdlib::DataType mt_data_type = mt::stdlib::DataType::BOOL;
    using type = mt::stdlib::type_info<mt_data_type>::type_t;
};

template <> struct data_type_t<DataType::DATA_TYPE> {
    using type = DataType;
    static const bool has_value = true;
    static const bool is_numeric = false;
    static const bool is_modelable = false;
};

template <> struct data_type_t<DataType::IDENTIFIER> {
    using type = tmdl::Identifier;
    static const bool has_value = true;
    static const bool is_numeric = false;
    static const bool modelable = false;
};

std::string data_type_to_string(const DataType dtype);

DataType data_type_from_string(const std::string& s);

}

#endif // TF_MODEL_DATA_TYPES_H
