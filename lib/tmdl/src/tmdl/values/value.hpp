// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VALUE_HPP
#define TF_MODEL_VALUE_HPP

#include <cstdint>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "../model_exception.hpp"

#include "data_types.hpp"
#include "mtstdlib_string.hpp"

#include <fmt/format.h>

namespace tmdl {

template <DataType> class ModelValueBox;

struct ModelValue {
    virtual ~ModelValue() = default;

    virtual DataType data_type() const = 0;

    virtual void copy_from(const ModelValue* value) = 0;

    virtual void copy_from(const mt::stdlib::Argument* arg) = 0;

    virtual std::unique_ptr<ModelValue> clone() const = 0;

    virtual std::string to_string() const = 0;

    virtual std::unique_ptr<mt::stdlib::Argument> to_argument() const = 0;

    static std::unique_ptr<ModelValue> make_default(const DataType dtype);

    static std::unique_ptr<ModelValue> from_string(std::string_view s, const DataType dt);

    static std::unique_ptr<ModelValue> convert_type(const ModelValue* val, const DataType dt);

    template <DataType DT> static const typename data_type_t<DT>::type_t& get_inner_value(const ModelValue* value) {
        const auto* ptr = dynamic_cast<const ModelValueBox<DT>*>(value);
        if (ptr == nullptr) {
            throw ModelException("unable to convert data type parameters");
        }
        return ptr->value;
    }

    template <DataType DT> static typename data_type_t<DT>::type_t& get_inner_value(ModelValue* value) {
        auto* ptr = dynamic_cast<ModelValueBox<DT>*>(value);
        if (ptr == nullptr) {
            throw ModelException("unable to convert data type parameters");
        }
        return ptr->value;
    }

    template <typename T> static std::unique_ptr<ModelValue> from_value(T val) {
        using namespace mt::stdlib;

        if constexpr (std::is_same_v<T, type_info<DataType::F64>::type_t>) {
            return std::make_unique<ModelValueBox<DataType::F64>>(val);
        } else if constexpr (std::is_same_v<T, type_info<DataType::F32>::type_t>) {
            return std::make_unique<ModelValueBox<DataType::F32>>(val);
        } else if constexpr (std::is_same_v<T, type_info<DataType::U8>::type_t>) {
            return std::make_unique<ModelValueBox<DataType::U8>>(val);
        } else if constexpr (std::is_same_v<T, type_info<DataType::U16>::type_t>) {
            return std::make_unique<ModelValueBox<DataType::U16>>(val);
        } else if constexpr (std::is_same_v<T, type_info<DataType::U32>::type_t>) {
            return std::make_unique<ModelValueBox<DataType::U32>>(val);
        } else if constexpr (std::is_same_v<T, type_info<DataType::I8>::type_t>) {
            return std::make_unique<ModelValueBox<DataType::I8>>(val);
        } else if constexpr (std::is_same_v<T, type_info<DataType::I16>::type_t>) {
            return std::make_unique<ModelValueBox<DataType::I16>>(val);
        } else if constexpr (std::is_same_v<T, type_info<DataType::I32>::type_t>) {
            return std::make_unique<ModelValueBox<DataType::I32>>(val);
        } else if constexpr (std::is_same_v<T, type_info<DataType::BOOL>::type_t>) {
            return std::make_unique<ModelValueBox<DataType::BOOL>>(val);
        } else {
            return nullptr;
        }
    }
};

template <DataType DT> struct ModelValueBox : public ModelValue {
    using type_t = typename data_type_t<DT>::type_t;

    ModelValueBox() = default;

    explicit ModelValueBox(const type_t inval) : value(inval) {}

    DataType data_type() const override {
        static_assert(DT != DataType::NONE);
        return DT;
    }

    std::string to_string() const override { return fmt::format("{}", value); }

    std::unique_ptr<mt::stdlib::Argument> to_argument() const override { return std::make_unique<mt::stdlib::ArgumentBox<DT>>(value); }

    void copy_from(const mt::stdlib::Argument* arg) override {
        if (auto ptr = dynamic_cast<const mt::stdlib::ArgumentBox<DT>*>(arg)) {
            value = ptr->value;
        } else {
            throw ModelException("mismatch in data type - unable to copy value");
        }
    };

    void copy_from(const ModelValue* in) override {
        if (auto ptr = dynamic_cast<const ModelValueBox<DT>*>(in)) {
            value = ptr->value;
        } else {
            throw ModelException("mismatch in data type - unable to copy value");
        }
    }

    std::unique_ptr<ModelValue> clone() const override { return std::make_unique<ModelValueBox<DT>>(value); }

    type_t value{};
};

template <> struct ModelValueBox<DataType::NONE> : public ModelValue {
    ModelValueBox() = default;

    DataType data_type() const override { return DataType::NONE; }

    std::string to_string() const override { return "??"; }

    std::unique_ptr<mt::stdlib::Argument> to_argument() const override { return nullptr; }

    void copy_from(const mt::stdlib::Argument* arg) override {
        if (arg->get_type() != DataType::NONE) {
            throw ModelException("argument is not a none-type");
        }
    };

    void copy_from(const ModelValue* in) override {
        throw ModelException(fmt::format("unable to copy from {} into an unknown data type", mt::stdlib::datatype_to_string(data_type())));
    }

    std::unique_ptr<ModelValue> clone() const override { return std::make_unique<ModelValueBox<DataType::NONE>>(); }
};

}

#endif // TF_MODEL_VALUE_HPP
