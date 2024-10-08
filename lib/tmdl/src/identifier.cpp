// SPDX-License-Identifier: GPL-3.0-only

#include "identifier.hpp"

#include "model_exception.hpp"

#include <fmt/format.h>

tmdl::Identifier::Identifier(const std::string_view s) { set(s); }

const std::string& tmdl::Identifier::get() const { return _value; }

void tmdl::Identifier::set(const std::string_view s) {
    if (!is_valid_identifier(s)) {
        throw ModelException(fmt::format("'{}' is not a valid identifier", s));
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
