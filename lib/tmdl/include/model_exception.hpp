// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_EXCEPTION_HPP
#define TF_MODEL_EXCEPTION_HPP

#include <string_view>
#include <stdexcept>

#include "mtstdlib_except.hpp"

namespace tmdl {

class ModelException : public std::runtime_error {
public:
    explicit ModelException(std::string_view msg);

    explicit ModelException(const mt::stdlib::block_error& err);

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

#endif // TF_MODEL_EXCEPTION_HPP
