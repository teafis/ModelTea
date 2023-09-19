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

#include "data_types.hpp"

#include <fmt/format.h>


namespace tmdl
{

template <DataType>
class ModelValueBox;

struct ModelValue
{
    virtual ~ModelValue() { }

    virtual DataType data_type() const = 0;

    virtual void copy_from(const ModelValue* value) = 0;

    virtual std::unique_ptr<ModelValue> clone() const = 0;

    virtual std::string to_string() const = 0;

    static std::unique_ptr<ModelValue> make_default(const DataType dtype);

    static std::unique_ptr<ModelValue> from_string(const std::string& s, const DataType dt);

    static std::unique_ptr<ModelValue> convert_type(const ModelValue* val, const DataType dt);

    template <DataType DT>
    static const typename data_type_t<DT>::type& get_inner_value(const ModelValue* value)
    {
        const auto* ptr = dynamic_cast<const ModelValueBox<DT>*>(value);
        if (ptr == nullptr)
        {
            throw ModelException("unable to convert data type parameters");
        }
        return ptr->value;
    }

    template <DataType DT>
    static typename data_type_t<DT>::type& get_inner_value(ModelValue* value)
    {
        auto* ptr = dynamic_cast<ModelValueBox<DT>*>(value);
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

    ModelValueBox() = default;

    ModelValueBox(const type_t inval) : value(inval) { }

    virtual DataType data_type() const override
    {
        static_assert(DT != DataType::UNKNOWN);
        return DT;
    }

    virtual std::string to_string() const override
    {
        if constexpr (DT == DataType::DATA_TYPE)
        {
            return data_type_to_string(value);
        }
        else if constexpr (DT == DataType::IDENTIFIER)
        {
            return fmt::format("\"{}\"", value.get());
        }
        else
        {
            return std::to_string(value);
        }
    }

    virtual void copy_from(const ModelValue* in) override
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

    virtual std::unique_ptr<ModelValue> clone() const override
    {
        return std::make_unique<ModelValueBox<DT>>(value);
    }

    type_t value{};
};

template <>
struct ModelValueBox<DataType::UNKNOWN> : public ModelValue
{
    ModelValueBox() = default;

    virtual DataType data_type() const override
    {
        return DataType::UNKNOWN;
    }

    virtual std::string to_string() const override
    {
        return "??";
    }

    virtual void copy_from(const ModelValue* in) override
    {
        // Do Nothing?
    }

    virtual std::unique_ptr<ModelValue> clone() const override
    {
        return std::make_unique<ModelValueBox<DataType::UNKNOWN>>();
    }
};

}

#endif // TF_MODEL_VALUE_HPP
