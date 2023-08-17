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


namespace tmdl
{

template <DataType>
class ModelValueBox;

struct ModelValue
{
    virtual ~ModelValue() { }

    virtual DataType data_type() const = 0;

    virtual void copy_value(const ModelValue* value) = 0;

    virtual std::string to_string() const = 0;

    static ModelValue* make_default(const DataType dtype);

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

    ModelValueBox() = default;

    ModelValueBox(const type_t inval) : value(inval) { }

    virtual DataType data_type() const override
    {
        return DT;
    }

    virtual std::string to_string() const override
    {
        return std::to_string(value);
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

    type_t value{};
};

}

#endif // TF_MODEL_VALUE_HPP
