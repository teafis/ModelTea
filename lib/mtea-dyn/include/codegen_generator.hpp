// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNCODEGEN_GENERATOR_HPP
#define MTEA_DYNCODEGEN_GENERATOR_HPP

#include <filesystem>

#include "block_interface.hpp"

namespace mtea::codegen {

class CodeGenerator {
public:
    explicit CodeGenerator(std::unique_ptr<CompiledBlockInterface>&& comp);

    void write_in_folder(const std::filesystem::path& path) const;

private:
    std::unique_ptr<CompiledBlockInterface> compiled;
};

}

#endif // MTEA_DYNCODEGEN_GENERATOR_HPP
