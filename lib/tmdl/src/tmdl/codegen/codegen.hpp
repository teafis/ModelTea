// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CODEGEN_HPP
#define TF_MODEL_CODEGEN_HPP

#include <exception>
#include <string>

#include "../value.hpp"

namespace tmdl::codegen
{

enum class CodeType
{
    CPP = 0,
};

enum class FunctionType
{
    INIT = 0,
    CLOSE,
    RESET,
    STEP,
    POST_STEP,
};

class CodegenError : public std::exception
{
public:
    CodegenError(const std::string& msg);

    virtual const char* what() const noexcept override;

protected:
    const std::string msg;
};

std::string get_datatype_name(CodeType code, tmdl::DataType datatype);

}

#endif // TF_MODEL_CODEGEN_HPP
