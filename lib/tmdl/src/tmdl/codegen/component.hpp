// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CODEGEN_COMPONENT_HPP
#define TF_MODEL_CODEGEN_COMPONENT_HPP

#include <filesystem>
#include <string>

#include "codegen.hpp"

namespace tmdl::codegen
{

class CodeComponent
{
public:
    virtual void write_code(CodeType type, std::filesystem::path folder) const = 0;

    virtual std::string get_file_name() const = 0;

    virtual std::string get_type_name() const = 0;

    virtual std::string get_function_name(FunctionType ft) const = 0;
};

}

#endif // TF_MODEL_CODEGEN_COMPONENT_HPP
