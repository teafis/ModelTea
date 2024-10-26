// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNCODEGEN_COMPONENT_HPP
#define MTEA_DYNCODEGEN_COMPONENT_HPP

#include <optional>
#include <string>
#include <vector>

#include "codegen.hpp"

namespace mtea::codegen {

class InterfaceDefinition {
public:
    explicit InterfaceDefinition(std::string_view type_name, const std::vector<std::string>& fields) : name(type_name), fields(fields) {
        // Empty Constructor
    }

    const std::string& get_name() const { return name; }

    size_t get_size() const { return fields.size(); }

    const std::string& get_field(const size_t i) const { return fields.at(i); }

private:
    std::string name;
    std::vector<std::string> fields;
};

class CodeComponent {
public:
    CodeComponent() = default;

    virtual ~CodeComponent() = default;

    virtual bool is_virtual() const;

    virtual std::optional<InterfaceDefinition> get_input_type() const = 0;

    virtual std::optional<InterfaceDefinition> get_output_type() const = 0;

    virtual std::vector<std::string> write_code(CodeSection section) const = 0;

    virtual std::string get_name_base() const = 0;

    virtual std::string get_module_name() const;

    virtual std::string get_type_name() const;

    virtual std::optional<std::string> get_function_name(BlockFunction ft) const = 0;

    virtual std::vector<std::string> constructor_arguments() const;
};

}

#endif // MTEA_DYNCODEGEN_COMPONENT_HPP
