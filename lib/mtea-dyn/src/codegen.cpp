// SPDX-License-Identifier: GPL-3.0-only

#include "codegen.hpp"

mtea::codegen::CodegenError::CodegenError(const std::string_view msg) : _msg(msg) {
    // Empty Constructor
}

const char* mtea::codegen::CodegenError::what() const noexcept { return _msg.c_str(); }

std::string mtea::codegen::get_datatype_name(const mtea::DataType datatype) {
    switch (datatype) {
        using enum mtea::DataType;
    case BOOL:
        return "bool";
    case I8:
        return "int8_t";
    case U8:
        return "uint8_t";
    case I16:
        return "int16_t";
    case U16:
        return "uint16_t";
    case I32:
        return "int32_t";
    case U32:
        return "uint32_t";
    case I64:
        return "int64_t";
    case U64:
        return "uint64_t";
    case F32:
        return "float";
    case F64:
        return "double";
    default:
        throw CodegenError("type not supported for C++ code");
    }
}
