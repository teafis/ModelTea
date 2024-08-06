// SPDX-License-Identifier: GPL-3.0-only

module;

#include <stdexcept>
#include <string>
#include <fmt/format.h>

export module tmdl.values:identifier;

namespace tmdl {

export class Identifier {
public:
    Identifier(const Identifier&) = default;

    explicit Identifier(std::string_view s);

    Identifier(Identifier&& other) = default;

    ~Identifier() = default;

    Identifier& operator=(const Identifier&) = default;

    bool operator==(const Identifier&) const = default;

    const std::string& get() const;

    void set(std::string_view s);

private:
    std::string _value;

public:
    static bool is_valid_identifier(std::string_view s);
};

}

tmdl::Identifier::Identifier(const std::string_view s) { set(s); }

const std::string& tmdl::Identifier::get() const { return _value; }

void tmdl::Identifier::set(const std::string_view s) {
    if (!is_valid_identifier(s)) {
        throw std::runtime_error(fmt::format("'{}' is not a valid identifier", s)); // TODO ModelException
    }

    _value = s;
}

bool tmdl::Identifier::is_valid_identifier(const std::string_view s) {
    for (const auto c : s) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }

    return !s.empty() && std::isalpha(s[0]);
}
