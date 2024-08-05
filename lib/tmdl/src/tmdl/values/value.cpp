// SPDX-License-Identifier: GPL-3.0-only

module;

#include <cstdint>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "mtstdlib_string.hpp"

#include <fmt/format.h>

export module tmdl.values:value;

import :data_types;

namespace tmdl {

export template <DataType> class ModelValueBox;

export struct ModelValue {
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

export template <DataType DT> struct ModelValueBox : public ModelValue {
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

export template <> struct ModelValueBox<DataType::NONE> : public ModelValue {
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

template <tmdl::DataType DT> static std::unique_ptr<tmdl::ModelValue> make_default_static() {
    return std::make_unique<tmdl::ModelValueBox<DT>>();
}

std::unique_ptr<tmdl::ModelValue> tmdl::ModelValue::make_default(const DataType dtype) {
    switch (dtype) {
        using enum tmdl::DataType;
    case BOOL:
        return make_default_static<BOOL>();
    case F32:
        return make_default_static<F32>();
    case F64:
        return make_default_static<F64>();
    case I8:
        return make_default_static<I8>();
    case U8:
        return make_default_static<U8>();
    case I16:
        return make_default_static<I16>();
    case U16:
        return make_default_static<U16>();
    case I32:
        return make_default_static<I32>();
    case U32:
        return make_default_static<U32>();
    case I64:
        return make_default_static<I64>();
    case U64:
        return make_default_static<U64>();
    case NONE:
        return make_default_static<NONE>();
    default:
        throw ModelException(fmt::format("unable to construct value for type {}", mt::stdlib::datatype_to_string(dtype)));
    }
}

std::unique_ptr<tmdl::ModelValue> tmdl::ModelValue::from_string(std::string_view s, const DataType dt) {
    const std::string ss(s);
    try {
        switch (dt) {
            using enum tmdl::DataType;
        case BOOL:
            return std::make_unique<ModelValueBox<BOOL>>(std::stoi(ss) != 0);
        case I8:
            return std::make_unique<ModelValueBox<I8>>(std::stoi(ss));
        case U8:
            return std::make_unique<ModelValueBox<U8>>(std::stoul(ss));
        case I16:
            return std::make_unique<ModelValueBox<I16>>(std::stoi(ss));
        case U16:
            return std::make_unique<ModelValueBox<U16>>(std::stoul(ss));
        case I32:
            return std::make_unique<ModelValueBox<I32>>(std::stoi(ss));
        case U32:
            return std::make_unique<ModelValueBox<U32>>(std::stoul(ss));
        case I64:
            return std::make_unique<ModelValueBox<I64>>(std::stoll(ss));
        case U64:
            return std::make_unique<ModelValueBox<U64>>(std::stoull(ss));
        case F32:
            return std::make_unique<ModelValueBox<F32>>(std::stof(ss));
        case F64:
            return std::make_unique<ModelValueBox<F64>>(std::stod(ss));
        case NONE:
            return make_default_static<NONE>();
        default:
            throw ModelException("unknown parse parameter type provided");
        }
    } catch (const std::invalid_argument&) {
        throw ModelException("error parsing parameter - invalid argument");
    } catch (const std::out_of_range&) {
        throw ModelException("error parsing parameter - out of range");
    }
}

template <tmdl::DataType DT>
static std::unique_ptr<tmdl::ModelValue> convert_numeric_type_helper(const tmdl::ModelValueBox<DT>* ptr, const tmdl::DataType dt) {
    switch (dt) {
        using enum tmdl::DataType;
    case BOOL:
        if constexpr (DT == BOOL) {
            return std::make_unique<tmdl::ModelValueBox<BOOL>>(ptr->value);
        } else {
            return std::make_unique<tmdl::ModelValueBox<BOOL>>(ptr->value != 0);
        }
    case I8:
        return std::make_unique<tmdl::ModelValueBox<I8>>(ptr->value);
    case U8:
        return std::make_unique<tmdl::ModelValueBox<U8>>(ptr->value);
    case I16:
        return std::make_unique<tmdl::ModelValueBox<I16>>(ptr->value);
    case U16:
        return std::make_unique<tmdl::ModelValueBox<U16>>(ptr->value);
    case I32:
        return std::make_unique<tmdl::ModelValueBox<I32>>(ptr->value);
    case U32:
        return std::make_unique<tmdl::ModelValueBox<U32>>(ptr->value);
    case I64:
        return std::make_unique<tmdl::ModelValueBox<I64>>(ptr->value);
    case U64:
        return std::make_unique<tmdl::ModelValueBox<U64>>(ptr->value);
    case F32:
        return std::make_unique<tmdl::ModelValueBox<F32>>(ptr->value);
    case F64:
        return std::make_unique<tmdl::ModelValueBox<F64>>(ptr->value);
    default:
        throw tmdl::ModelException("unsupported data type provided");
    }
}

std::unique_ptr<tmdl::ModelValue> tmdl::ModelValue::convert_type(const ModelValue* val, const DataType dt) {
    if (val == nullptr)
        throw ModelException("unexpected nullptr");

    using enum DataType;

    if (auto ptr_bool = dynamic_cast<const ModelValueBox<BOOL>*>(val)) {
        return convert_numeric_type_helper<BOOL>(ptr_bool, dt);
    } else if (auto ptr_i8 = dynamic_cast<const ModelValueBox<I8>*>(val)) {
        return convert_numeric_type_helper<I8>(ptr_i8, dt);
    } else if (auto ptr_u8 = dynamic_cast<const ModelValueBox<U8>*>(val)) {
        return convert_numeric_type_helper<U8>(ptr_u8, dt);
    } else if (auto ptr_i16 = dynamic_cast<const ModelValueBox<I16>*>(val)) {
        return convert_numeric_type_helper<I16>(ptr_i16, dt);
    } else if (auto ptr_u16 = dynamic_cast<const ModelValueBox<U16>*>(val)) {
        return convert_numeric_type_helper<U16>(ptr_u16, dt);
    } else if (auto ptr_i16 = dynamic_cast<const ModelValueBox<I16>*>(val)) {
        return convert_numeric_type_helper<I16>(ptr_i16, dt);
    } else if (auto ptr_i32 = dynamic_cast<const ModelValueBox<I32>*>(val)) {
        return convert_numeric_type_helper<I32>(ptr_i32, dt);
    } else if (auto ptr_u32 = dynamic_cast<const ModelValueBox<U32>*>(val)) {
        return convert_numeric_type_helper<U32>(ptr_u32, dt);
    } else if (auto ptr_i64 = dynamic_cast<const ModelValueBox<I64>*>(val)) {
        return convert_numeric_type_helper<I64>(ptr_i64, dt);
    } else if (auto ptr_u64 = dynamic_cast<const ModelValueBox<U64>*>(val)) {
        return convert_numeric_type_helper<U64>(ptr_u64, dt);
    } else if (auto ptr_f32 = dynamic_cast<const ModelValueBox<F32>*>(val)) {
        return convert_numeric_type_helper<F32>(ptr_f32, dt);
    } else if (auto ptr_f64 = dynamic_cast<const ModelValueBox<F64>*>(val)) {
        return convert_numeric_type_helper<F64>(ptr_f64, dt);
    } else {
        throw ModelException("unsupported input value type");
    }
}
