// SPDX-License-Identifier: GPL-3.0-only

#include "codegen_component.hpp"

#include <fmt/format.h>

bool mtea::codegen::CodeComponent::is_virtual() const { return false; }

std::string mtea::codegen::CodeComponent::get_module_name() const { return fmt::format("{}.h", get_name_base()); }

std::string mtea::codegen::CodeComponent::get_type_name() const { return get_name_base(); }

std::vector<std::string> mtea::codegen::CodeComponent::constructor_arguments() const { return {}; }

std::vector<std::string> mtea::codegen::CodeComponent::write_code(CodeSection section) const {
    return {};
}
