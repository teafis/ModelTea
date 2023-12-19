#include "component.hpp"

#include <fmt/format.h>

bool tmdl::codegen::CodeComponent::is_virtual() const { return false; }

std::string tmdl::codegen::CodeComponent::get_module_name() const { return fmt::format("{}.h", get_name_base()); }

std::string tmdl::codegen::CodeComponent::get_type_name() const { return get_name_base(); }

std::vector<std::string> tmdl::codegen::CodeComponent::constructor_arguments() const { return {}; }

std::vector<std::string> tmdl::codegen::CodeComponent::write_code(Language type, CodeSection section) const {
    if (type == Language::CPP) {
        return write_cpp_code(section);
    } else {
        throw CodegenError("provided code type not supported");
    }
}

std::vector<std::string> tmdl::codegen::CodeComponent::write_cpp_code(CodeSection) const { return {}; }
