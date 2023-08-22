// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_IDENTIFIERS_HPP
#define TF_MODEL_IDENTIFIERS_HPP

#include <string>

namespace tmdl
{

class Identifier
{
public:
    Identifier(const Identifier&) = default;

    Identifier(const std::string& s);

    Identifier(Identifier&& other) = default;

    ~Identifier() = default;

    Identifier& operator=(const Identifier&) = default;

    bool operator==(const Identifier&) const = default;

    const std::string& get() const;

    void set(const std::string& s);

private:
    static bool is_valid_identifier(const std::string& s);

private:
    std::string _value;
};

}

#endif // TF_MODEL_IDENTIFIERS_HPP
