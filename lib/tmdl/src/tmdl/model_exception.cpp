// SPDX-License-Identifier: GPL-3.0-only

module;

#include <string_view>
#include <stdexcept>

#include "mtstdlib_except.hpp"

export module tmdl:model_exception;

namespace tmdl {

class ModelException : public std::runtime_error {
public:
    explicit ModelException(std::string_view msg) : std::runtime_error(std::string(msg)) {}

    explicit ModelException(const mt::stdlib::block_error& err) : ModelException(std::string_view(err.what())) {}

    virtual ~ModelException() = default;
};

class ExecutionException : public ModelException {
public:
    ExecutionException(std::string_view msg, const size_t id) : ModelException(msg), _id(id) {}

    size_t get_id() const { return _id; }

private:
    size_t _id;
};

}
