// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VALUE_HPP
#define TF_MODEL_VALUE_HPP

#include <cstdint>

#include <string>
#include <memory>
#include <optional>


namespace tmdl
{

enum class DataType : uint32_t
{
    UNKNOWN = 0,
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

struct ModelValue
{
    virtual ~ModelValue()
    {
        // Empty Destructor
    }
};

template <DataType DT>
struct ModelValueBox : public ModelValue
{
    using type_t = data_type_t<DT>::type;

    ModelValueBox() : value{}
    {
        // Empty Constructor
    }

    ModelValueBox(const type_t inval) : value{inval}
    {
        // Empty Constructor
    }

    type_t value;
};

std::shared_ptr<ModelValue> make_shared_default_value(const DataType dtype);

}

#endif // TF_MODEL_VALUE_HPP
