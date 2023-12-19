// SPDX-License-Identifier: GPL-3.0-only

#ifndef TEAFIS_EXCEPTION_H
#define TEAFIS_EXCEPTION_H

#include <stdexcept>

class TeafisException : public std::runtime_error {
public:
    TeafisException(const std::string& s) : std::runtime_error(s) {}
    TeafisException(const char* c) : std::runtime_error(c) {}
};

#endif // TEAFIS_EXCEPTION_H
