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
    Language get_language() const
    {
        return Language::CPP;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<CodeComponent>> components;

    //size_t var_count = 0;
};

}

#endif // TF_MODEL_CODEGEN_GENERATOR_HPP
