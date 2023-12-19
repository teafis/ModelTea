// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CODEGEN_COMPONENT_HPP
#define TF_MODEL_CODEGEN_COMPONENT_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "codegen.hpp"

namespace tmdl::codegen {

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

    virtual std::optional<const InterfaceDefinition> get_input_type() const = 0;

    virtual std::optional<const InterfaceDefinition> get_output_type() const = 0;

    std::vector<std::string> write_code(Language type, CodeSection section) const;

    virtual std::string get_name_base() const = 0;

    virtual std::string get_module_name() const;

    virtual std::string get_type_name() const;

    virtual std::optional<std::string> get_function_name(BlockFunction ft) const = 0;

    virtual std::vector<std::string> constructor_arguments() const;

protected:
    virtual std::vector<std::string> write_cpp_code(CodeSection section) const;
};

}

#endif // TF_MODEL_CODEGEN_COMPONENT_HPP
