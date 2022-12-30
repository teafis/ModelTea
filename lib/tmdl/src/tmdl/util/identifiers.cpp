// SPDX-License-Identifier: GPL-3.0-only

#include "identifiers.hpp"

bool tmdl::is_valid_identifier(const std::string& s)
{
    for (const auto c : s)
    {
        if (!std::isalnum(c) && c != '_')
        {
            return false;
        }
    }

    return !s.empty() && std::isalpha(s[0]);
}
