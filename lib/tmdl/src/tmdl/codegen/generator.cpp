module;

#include "generator.hpp"

#include <filesystem>
#include <string>
#include <unordered_map>

#include "../block_interface.hpp"

#include <fmt/format.h>

export module tmdl.codegen:generator;

namespace tmdl::codegen {

export class CodeGenerator {
public:
    explicit CodeGenerator(std::unique_ptr<CompiledBlockInterface>&& comp);

    Language get_language() const;

    void write_in_folder(const std::filesystem::path& path) const;

private:
    std::unique_ptr<CompiledBlockInterface> compiled;
};

}

tmdl::codegen::CodeGenerator::CodeGenerator(std::unique_ptr<CompiledBlockInterface>&& comp) : compiled(std::move(comp)) {
    // Empty Constructor
}

tmdl::codegen::Language tmdl::codegen::CodeGenerator::get_language() const { return Language::CPP; }

void tmdl::codegen::CodeGenerator::write_in_folder(const std::filesystem::path& path) const {
    const std::vector<tmdl::codegen::CodeSection> sections{
        tmdl::codegen::CodeSection::DEFINITION,
        tmdl::codegen::CodeSection::DECLARATION,
    };

    const std::vector<std::unique_ptr<tmdl::codegen::CodeComponent>> components = compiled->get_codegen_components();

    for (const auto& c : components) {
        for (const auto& sec : sections) {
            const auto code = c->write_code(tmdl::codegen::Language::CPP, sec);
            if (code.empty()) {
                continue;
            }

            std::string file_ext;
            if (sec == tmdl::codegen::CodeSection::DEFINITION) {
                file_ext = "cpp";
            } else if (sec == tmdl::codegen::CodeSection::DECLARATION) {
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
