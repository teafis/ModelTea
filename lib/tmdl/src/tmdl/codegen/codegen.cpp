#include "codegen.hpp"

tmdl::codegen::CodegenError::CodegenError(const std::string_view msg) : _msg(msg) {
    // Empty Constructor
}

const char* tmdl::codegen::CodegenError::what() const noexcept { return _msg.c_str(); }

std::string tmdl::codegen::get_datatype_name(const Language code, const tmdl::DataType datatype) {
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
