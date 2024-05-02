// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_EXCEPTION_HPP
#define TF_MODEL_EXCEPTION_HPP

#include <string_view>
#include <stdexcept>

namespace tmdl {

class ModelException : public std::runtime_error {
public:
    explicit ModelException(std::string_view msg);

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
