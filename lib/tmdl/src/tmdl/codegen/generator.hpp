// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CODEGEN_GENERATOR_HPP
#define TF_MODEL_CODEGEN_GENERATOR_HPP

#include <filesystem>
#include <string>
#include <unordered_map>

#include "../block_interface.hpp"

namespace tmdl::codegen
{

class CodeGenerator
{
public:
    CodeGenerator(std::unique_ptr<CompiledBlockInterface>&& comp);

    Language get_language() const;

    void write_in_folder(const std::filesystem::path& path) const;

private:
    std::unique_ptr<CompiledBlockInterface> compiled;

    //size_t var_count = 0;
};

}

#endif // TF_MODEL_CODEGEN_GENERATOR_HPP
