// SPDX-License-Identifier: GPL-3.0-only

#include "value.hpp"
#include "../model_exception.hpp"

#include <utility>

template <tmdl::DataType DT> static std::unique_ptr<tmdl::ModelValue> make_default_static() {
    return std::make_unique<tmdl::ModelValueBox<DT>>();
}

std::unique_ptr<tmdl::ModelValue> tmdl::ModelValue::make_default(const DataType dtype) {
    switch (dtype) {
        using enum tmdl::DataType;
    case BOOLEAN:
        return make_default_static<BOOLEAN>();
    case DOUBLE:
        return make_default_static<DOUBLE>();
    case SINGLE:
        return make_default_static<SINGLE>();
    case INT32:
        return make_default_static<INT32>();
    case UINT32:
        return make_default_static<UINT32>();
    case DATA_TYPE:
        return make_default_static<DATA_TYPE>();
    case UNKNOWN:
        return make_default_static<UNKNOWN>();
    default:
        throw ModelException(fmt::format("unable to construct value for type {}", data_type_to_string(dtype)));
    }
}

std::unique_ptr<tmdl::ModelValue> tmdl::ModelValue::from_string(const std::string& s, const DataType dt) {
    try {
        switch (dt) {
            using enum tmdl::DataType;
        case BOOLEAN:
            return std::make_unique<ModelValueBox<BOOLEAN>>(std::stoi(s) != 0);
        case INT32:
            return std::make_unique<ModelValueBox<INT32>>(std::stoi(s));
        case UINT32:
            return std::make_unique<ModelValueBox<UINT32>>(std::stoul(s));
        case SINGLE:
            return std::make_unique<ModelValueBox<SINGLE>>(std::stof(s));
        case DOUBLE:
            return std::make_unique<ModelValueBox<DOUBLE>>(std::stod(s));
        case DATA_TYPE:
            return std::make_unique<ModelValueBox<DATA_TYPE>>(data_type_from_string(s));
        case IDENTIFIER:
            return std::make_unique<ModelValueBox<IDENTIFIER>>(Identifier(s));
        case UNKNOWN:
            return make_default_static<UNKNOWN>();
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
    case BOOLEAN:
        if constexpr (DT == BOOLEAN) {
            return std::make_unique<tmdl::ModelValueBox<BOOLEAN>>(ptr->value);
        } else {
            return std::make_unique<tmdl::ModelValueBox<BOOLEAN>>(ptr->value != 0);
        }
    case INT32:
        return std::make_unique<tmdl::ModelValueBox<INT32>>(ptr->value);
    case UINT32:
        return std::make_unique<tmdl::ModelValueBox<UINT32>>(ptr->value);
    case SINGLE:
        return std::make_unique<tmdl::ModelValueBox<SINGLE>>(ptr->value);
    case DOUBLE:
        return std::make_unique<tmdl::ModelValueBox<DOUBLE>>(ptr->value);
    default:
        throw tmdl::ModelException("unsupported data type provided");
    }
}

std::unique_ptr<tmdl::ModelValue> tmdl::ModelValue::convert_type(const ModelValue* val, const DataType dt) {
    if (val == nullptr)
        throw ModelException("unexpected nullptr");

    using enum DataType;

    if (auto ptr_bool = dynamic_cast<const ModelValueBox<BOOLEAN>*>(val)) {
        return convert_numeric_type_helper<BOOLEAN>(ptr_bool, dt);
    } else if (auto ptr_i32 = dynamic_cast<const ModelValueBox<INT32>*>(val)) {
        return convert_numeric_type_helper<INT32>(ptr_i32, dt);
    } else if (auto ptr_u32 = dynamic_cast<const ModelValueBox<UINT32>*>(val)) {
        return convert_numeric_type_helper<UINT32>(ptr_u32, dt);
    } else if (auto ptr_f32 = dynamic_cast<const ModelValueBox<SINGLE>*>(val)) {
        return convert_numeric_type_helper<SINGLE>(ptr_f32, dt);
    } else if (auto ptr_f64 = dynamic_cast<const ModelValueBox<DOUBLE>*>(val)) {
        return convert_numeric_type_helper<DOUBLE>(ptr_f64, dt);
    } else {
        throw ModelException("unsupported input value type");
    }
}
