// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNCODEGEN_HPP
#define MTEA_DYNCODEGEN_HPP

#include <string>

#include "data_type.hpp"

namespace mtea::codegen {

enum class CodeSection {
    DEFINITION = 0,
    DECLARATION,
};

enum class BlockFunction {
    RESET = 0,
    STEP,
};

class CodegenError {
public:
    explicit CodegenError(std::string_view msg);

    virtual ~CodegenError() = default;

    virtual const char* what() const noexcept;

protected:
    const std::string _msg;
};

std::string get_datatype_name(mtea::DataType datatype);

}

#endif // MTEA_DYNCODEGEN_HPP
