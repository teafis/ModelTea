// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CODE_GENERATOR_HPP
#define TF_MODEL_CODE_GENERATOR_HPP

#include <ostream>
#include <string>
#include <vector>

namespace tmdl::codegen
{

enum class CodeType
{
    CPP = 0,
};

class CodeComponent
{
public:
    std::ostream& write_declaration(std::ostream& s)
    {
        for (const auto& l : declaration)
        {
            s << l << '\n';
        }
        return s;
    }

    std::ostream& write_definition(std::ostream& s)
    {
        for (const auto& l : definition)
        {
            s << l << '\n';
        }
        return s;
    }

    bool declaration_is_public() const
    {
        return true;
    }

protected:
    std::vector<std::string> declaration;
    std::vector<std::string> definition;
};

class CodeGenerator
{
public:

private:

};

}

#endif // TF_MODEL_CODE_GENERATOR_HPP
