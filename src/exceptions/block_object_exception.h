// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCK_OBJECT_EXCEPTION_H
#define BLOCK_OBJECT_EXCEPTION_H

#include "teafis_exception.h"

class BlockObjectException : public TeafisException
{
public:
    BlockObjectException(const std::string& s) : TeafisException(s) { }
    BlockObjectException(const char* c) : TeafisException(c) { }
};

#endif // BLOCK_OBJECT_EXCEPTION_H
