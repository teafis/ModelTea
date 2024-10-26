// SPDX-License-Identifier: GPL-3.0-only

#include "identifier.hpp"

#include "model_exception.hpp"

#include <fmt/format.h>

void mtea::to_json(nlohmann::json& j, const Identifier& i) {
    j["name"] = i._value;
}

void mtea::from_json(const nlohmann::json& j, Identifier& i) {
    j.at("name").get_to(i._value);
}

mtea::Identifier::Identifier(const std::string_view s) { set(s); }

const std::string& mtea::Identifier::get() const { return _value; }

void mtea::Identifier::set(const std::string_view s) {
    if (!is_valid_identifier(s)) {
        throw ModelException(fmt::format("'{}' is not a valid identifier", s));
    }

    _value = s;
}

bool mtea::Identifier::is_valid_identifier(const std::string_view s) {
    for (const auto c : s) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }

    return !s.empty() && std::isalpha(s[0]);
}

size_t mtea::Identifier::Hasher::operator()(const Identifier& other) const { return std::hash<std::string>{}(other.get()); }
