// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VALUE_HPP
#define TF_MODEL_VALUE_HPP

#include <cstdint>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../model_exception.hpp"


namespace tmdl
{

// TODO - Move all parameters into an array type? Or leave as single elements?
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

template <DataType>
class ModelValueBox;

struct ModelValue
{
    virtual DataType data_type() const = 0;

    virtual void copy_value(const ModelValue* value) = 0;

    virtual ~ModelValue()
    {
        // Empty Destructor
    }

    template <DataType DT>
    static ModelValue* make_default()
    {
        return new tmdl::ModelValueBox<DT>();
    }

    static ModelValue* make_default_type(const DataType dtype);

    static ModelValue* from_string(const std::string& s, const DataType dt);

    static ModelValue* convert_type(const ModelValue* val, const DataType dt);

    template <DataType DT>
    static typename data_type_t<DT>::type get_inner_value(const ModelValue* value)
    {
        const auto* ptr = dynamic_cast<const ModelValueBox<DT>*>(value);
        if (ptr == nullptr)
        {
            throw ModelException("unable to convert data type parameters");
        }
        return ptr->value;
    }
};

template <DataType DT>
struct ModelValueBox : public ModelValue
{
    using type_t = typename data_type_t<DT>::type;

    ModelValueBox() : value{}
    {
        // Empty Constructor
    }

    ModelValueBox(const type_t inval) : value{inval}
    {
        // Empty Constructor
    }

    virtual DataType data_type() const override
    {
        return DT;
    }

    virtual void copy_value(const ModelValue* in) override
    {
        if (auto ptr = dynamic_cast<const ModelValueBox<DT>*>(in))
        {
            value = ptr->value;
        }
        else
        {
            throw ModelException("mismatch in data type - unable to copy value");
        }
    }

    type_t value;
};

}

#endif // TF_MODEL_VALUE_HPP
