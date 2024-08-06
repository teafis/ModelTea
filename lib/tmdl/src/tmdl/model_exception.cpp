// SPDX-License-Identifier: GPL-3.0-only

module;

#include <string_view>
#include <stdexcept>

#include "mtstdlib_except.hpp"

export module tmdl:module_exception;

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

tmdl::ModelException::ModelException(std::string_view msg) : std::runtime_error(std::string(msg)) {}

tmdl::ModelException::ModelException(const mt::stdlib::block_error& err) : ModelException(std::string_view(err.what())) {}

tmdl::ExecutionException::ExecutionException(std::string_view msg, const size_t id) : ModelException(msg), _id(id) {}

size_t tmdl::ExecutionException::get_id() const { return _id; }
