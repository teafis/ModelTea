// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNIDENTIFIERS_HPP
#define MTEA_DYNIDENTIFIERS_HPP

#include <string>
#include <utility>

#include <nlohmann/json.hpp>

namespace mtea {

class Identifier;

void to_json(nlohmann::json& j, const Identifier& i);
void from_json(const nlohmann::json& j, Identifier& i);

class Identifier {
private:
    Identifier() = default;

public:
    Identifier(const Identifier&) = default;

    Identifier(std::string_view s);

    Identifier(Identifier&& other) = default;

    ~Identifier() = default;

    Identifier& operator=(const Identifier&) = default;

    bool operator==(const Identifier&) const = default;

    const std::string& get() const;

    void set(std::string_view s);

public:
    struct Hasher {
        size_t operator()(const Identifier& other) const;
    };

private:
    std::string _value;

public:
    static bool is_valid_identifier(std::string_view s);

    friend void to_json(nlohmann::json&, const Identifier&);
    friend void from_json(const nlohmann::json&, Identifier&);
};

}

#endif // MTEA_DYNIDENTIFIERS_HPP
