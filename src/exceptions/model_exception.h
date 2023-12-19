// SPDX-License-Identifier: GPL-3.0-only

#ifndef MODEL_EXCEPTION_H
#define MODEL_EXCEPTION_H

#include "teafis_exception.h"

class ModelException : public TeafisException {
public:
    explicit ModelException(const std::string& s) : TeafisException(s) {}
    explicit ModelException(const char* c) : TeafisException(c) {}
};

#endif // MODEL_EXCEPTION_H
