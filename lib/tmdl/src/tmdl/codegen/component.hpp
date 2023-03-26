// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CODEGEN_COMPONENT_HPP
#define TF_MODEL_CODEGEN_COMPONENT_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "codegen.hpp"

namespace tmdl::codegen
{

class CodeComponent
{
public:
    class InterfaceDefinition
    {
    public:
        InterfaceDefinition(const std::string& type_name, const std::vector<std::string>& fields) :
            name(type_name),
            fields(fields)
        {
            // Empty Constructor
        }

        const std::string& get_name() const
        {
            return name;
        }

        size_t get_size() const
        {
            return fields.size();
        }

        const std::string& get_field(const size_t i) const
        {
            return fields.at(i);
        }

    protected:
        std::string name;
        std::vector<std::string> fields;
    };

    virtual const InterfaceDefinition& get_input_type() const = 0;

    virtual const InterfaceDefinition& get_output_type() const = 0;

    virtual void write_code(CodeType type, std::filesystem::path folder) const = 0;

    virtual std::string get_file_name() const = 0;

    virtual std::string get_type_name() const = 0;

    virtual std::string get_input_struct_name() const = 0;

    virtual std::optional<std::string> get_function_name(FunctionType ft) const = 0;
};

}

#endif // TF_MODEL_CODEGEN_COMPONENT_HPP
