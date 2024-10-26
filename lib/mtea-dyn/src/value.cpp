// SPDX-License-Identifier: GPL-3.0-only

#include "value.hpp"

#include "model_exception.hpp"

#include "mtea_string.hpp"

template <mtea::DataType DT> static std::unique_ptr<mtea::ModelValue> make_default_static() {
    return std::make_unique<mtea::ModelValueBox<DT>>();
}

std::unique_ptr<mtea::ModelValue> mtea::ModelValue::make_default(const DataType dtype) {
    switch (dtype) {
        using enum mtea::DataType;
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
        throw ModelException(fmt::format("unable to construct value for type {}", mtea::datatype_to_string(dtype)));
    }
}

std::unique_ptr<mtea::ModelValue> mtea::ModelValue::from_string(std::string_view s, const DataType dt) {
    const std::string ss(s);
    try {
        switch (dt) {
            using enum mtea::DataType;
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

template <mtea::DataType DT>
static std::unique_ptr<mtea::ModelValue> convert_numeric_type_helper(const mtea::ModelValueBox<DT>* ptr, const mtea::DataType dt) {
    switch (dt) {
        using enum mtea::DataType;
    case BOOL:
        if constexpr (DT == BOOL) {
            return std::make_unique<mtea::ModelValueBox<BOOL>>(ptr->value);
        } else {
            return std::make_unique<mtea::ModelValueBox<BOOL>>(ptr->value != 0);
        }
    case I8:
        return std::make_unique<mtea::ModelValueBox<I8>>(ptr->value);
    case U8:
        return std::make_unique<mtea::ModelValueBox<U8>>(ptr->value);
    case I16:
        return std::make_unique<mtea::ModelValueBox<I16>>(ptr->value);
    case U16:
        return std::make_unique<mtea::ModelValueBox<U16>>(ptr->value);
    case I32:
        return std::make_unique<mtea::ModelValueBox<I32>>(ptr->value);
    case U32:
        return std::make_unique<mtea::ModelValueBox<U32>>(ptr->value);
    case I64:
        return std::make_unique<mtea::ModelValueBox<I64>>(ptr->value);
    case U64:
        return std::make_unique<mtea::ModelValueBox<U64>>(ptr->value);
    case F32:
        return std::make_unique<mtea::ModelValueBox<F32>>(ptr->value);
    case F64:
        return std::make_unique<mtea::ModelValueBox<F64>>(ptr->value);
    default:
        throw mtea::ModelException("unsupported data type provided");
    }
}

std::unique_ptr<mtea::ModelValue> mtea::ModelValue::convert_type(const ModelValue* val, const DataType dt) {
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
