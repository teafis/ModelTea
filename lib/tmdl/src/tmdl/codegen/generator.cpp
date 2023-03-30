#include "generator.hpp"

#include <fstream>

#include <fmt/format.h>

tmdl::codegen::Language tmdl::codegen::CodeGenerator::get_language() const
{
    return Language::CPP;
}

void tmdl::codegen::CodeGenerator::write_in_folder(const std::filesystem::path& path) const
{
    const auto lang = get_language();

    const std::string definition_ext = [lang]()
    {
        switch (lang)
        {
        case Language::CPP:
            return ".cpp";
        default:
            throw CodegenError("no extension provided for language");
        }
    }();

    for (const auto& i : components)
    {
        const auto& name = i.first;
        const auto& c = i.second;

        const auto write_file_info = [name](std::ostream& oss)
        {
            oss << "// " << name << '\n';
            oss << "// Generated Code!\n";
            oss << '\n';
        };

        const std::string file_base = c->get_name_base();

        if (lang == Language::CPP)
        {
            const std::string header_name = c->get_include_file_name();

            std::ofstream hdr(path / header_name);
            write_file_info(hdr);

            for (const auto& l : c->write_code(lang, CodeSection::DECLARATION))
            {
                hdr << l << '\n';
            }
        }

        {
            const std::string header_name = fmt::format("{}{}", file_base, definition_ext);

            std::ofstream impl(path / header_name);
            write_file_info(impl);

            for (const auto& l : c->write_code(lang, CodeSection::DEFINITION))
            {
                impl << l << '\n';
            }
        }
    }
}
