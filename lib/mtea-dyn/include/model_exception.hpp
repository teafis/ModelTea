// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNEXCEPTION_HPP
#define MTEA_DYNEXCEPTION_HPP

#include <string_view>
#include <stdexcept>

#include "mtea_except.hpp"

namespace mtea {

class ModelException : public std::runtime_error {
public:
    explicit ModelException(std::string_view msg);

    explicit ModelException(const mtea::block_error& err);

    virtual ~ModelException() = default;
};

class ExecutionException : public ModelException {
public:
    ExecutionException(std::string_view msg, const size_t id);

    size_t get_id() const;

private:
    size_t _id;
};

}

#endif // MTEA_DYNEXCEPTION_HPP
