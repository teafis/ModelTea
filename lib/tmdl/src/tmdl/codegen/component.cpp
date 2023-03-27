#include "component.hpp"

std::vector<std::string> tmdl::codegen::CodeComponent::write_code(Language type, CodeSection section) const
{
    if (type == Language::CPP)
    {
        return write_cpp_code(section);
    }
    else
    {
        throw CodegenError("provided code type not supported");
    }
}

std::vector<std::string> tmdl::codegen::write_cpp_code(CodeSection section) const
{
    throw CodegenError("C++ code generation not supported");
}
