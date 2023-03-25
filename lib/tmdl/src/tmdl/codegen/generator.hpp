// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CODEGEN_GENERATOR_HPP
#define TF_MODEL_CODEGEN_GENERATOR_HPP

#include <string>
#include <unordered_map>

#include "component.hpp"

namespace tmdl::codegen
{

class CodeGenerator
{
public:

private:
    std::unordered_map<std::string, std::unique_ptr<CodeComponent>> components;
};

}

#endif // TF_MODEL_CODEGEN_GENERATOR_HPP
