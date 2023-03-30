#include "component.hpp"

#include <fmt/format.h>

tmdl::codegen::CodeComponent::CodeComponent(const std::string& name) : _variable_name(name)
{
    // Empty Constructor
}

std::string tmdl::codegen::CodeComponent::get_include_file_name() const
{
    return fmt::format("{}.h", get_name_base());
}

std::string tmdl::codegen::CodeComponent::get_type_name() const
{
    return get_name_base();
}

std::string tmdl::codegen::CodeComponent::get_varname() const
{
    return _variable_name;
}

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

std::vector<std::string> tmdl::codegen::CodeComponent::write_cpp_code([[maybe_unused]] CodeSection section) const
{
    throw CodegenError("C++ code generation not supported");
}
