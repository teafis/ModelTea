#include "codegen.hpp"

tmdl::codegen::CodegenError::CodegenError(const std::string& msg) : _msg(msg)
{
    // Empty Constructor
}

const char* tmdl::codegen::CodegenError::what() const noexcept
{
    return _msg.c_str();
}

std::string tmdl::codegen::get_datatype_name(const Language code, const tmdl::DataType datatype)
{
    if (code == Language::CPP)
    {
        switch (datatype)
        {
        case tmdl::DataType::BOOLEAN:
            return "bool";
        case tmdl::DataType::INT32:
            return "int32_t";
        case tmdl::DataType::UINT32:
            return "uint32_t";
        case tmdl::DataType::SINGLE:
            return "float";
        case tmdl::DataType::DOUBLE:
            return "double";
        default:
            throw CodegenError("type not supported for C++ code");
        }
    }
    else
    {
        throw CodegenError("unknown code type provided");
    }
}
