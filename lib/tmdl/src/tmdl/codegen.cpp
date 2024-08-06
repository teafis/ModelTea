// SPDX-License-Identifier: GPL-3.0-only

module;

#include <stdexcept>
#include <string>
#include <string_view>

export module tmdl:codegen;

import tmdl.values;

namespace tmdl::codegen {

export enum class Language {
    CPP = 0,
};

export enum class CodeSection {
    DEFINITION = 0,
    DECLARATION,
};

export enum class BlockFunction {
    RESET = 0,
    STEP,
};

export class CodegenError : public std::runtime_error {
public:
    explicit CodegenError(std::string_view msg) : _msg {}

    virtual ~CodegenError() = default;

    virtual const char* what() const noexcept { return _msg.c_str(); }

protected:
    const std::string _msg;
};

export std::string tmdl::codegen::get_datatype_name(const Language code, const tmdl::DataType datatype) {
    if (code == Language::CPP) {
        switch (datatype) {
            using enum tmdl::DataType;
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
    } else {
        throw CodegenError("unknown code type provided");
    }
}

}
