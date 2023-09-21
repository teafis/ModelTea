// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CODEGEN_HPP
#define TF_MODEL_CODEGEN_HPP

#include <string>

#include "../values/data_types.hpp"

namespace tmdl::codegen
{

enum class Language
{
    CPP = 0,
};

enum class CodeSection
{
    DEFINITION = 0,
    DECLARATION,
};

enum class BlockFunction
{
    INIT = 0,
    RESET,
    STEP,
};

class CodegenError
{
public:
    explicit CodegenError(std::string_view msg);

    virtual ~CodegenError() = default;

    virtual const char* what() const noexcept;

protected:
    const std::string _msg;
};

std::string get_datatype_name(Language code, tmdl::DataType datatype);

}

#endif // TF_MODEL_CODEGEN_HPP
