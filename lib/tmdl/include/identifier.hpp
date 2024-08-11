// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_IDENTIFIERS_HPP
#define TF_MODEL_IDENTIFIERS_HPP

#include <string>

namespace tmdl {

class Identifier {
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

#endif // TF_MODEL_IDENTIFIERS_HPP
