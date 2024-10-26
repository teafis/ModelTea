// SPDX-License-Identifier: GPL-3.0-only

#include "codegen_generator.hpp"

#include <fstream>

#include <fmt/format.h>

mtea::codegen::CodeGenerator::CodeGenerator(std::unique_ptr<CompiledBlockInterface>&& comp) : compiled(std::move(comp)) {
    // Empty Constructor
}

void mtea::codegen::CodeGenerator::write_in_folder(const std::filesystem::path& path) const {
    const std::vector<mtea::codegen::CodeSection> sections{
        mtea::codegen::CodeSection::DEFINITION,
        mtea::codegen::CodeSection::DECLARATION,
    };

    const std::vector<std::unique_ptr<mtea::codegen::CodeComponent>> components = compiled->get_codegen_components();

    for (const auto& c : components) {
        for (const auto& sec : sections) {
            const auto code = c->write_code(sec);
            if (code.empty()) {
                continue;
            }

            std::string file_ext;
            if (sec == mtea::codegen::CodeSection::DEFINITION) {
                file_ext = "cpp";
            } else if (sec == mtea::codegen::CodeSection::DECLARATION) {
                file_ext = "h";
            } else {
                continue;
            }

            std::ofstream output(path / fmt::format("{}.{}", c->get_name_base(), file_ext));

            for (const auto& l : code) {
                output << l << std::endl;
            }
        }
    }
}
