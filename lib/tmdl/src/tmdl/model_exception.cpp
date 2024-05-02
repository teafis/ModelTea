// SPDX-License-Identifier: GPL-3.0-only

#include "model_exception.hpp"

tmdl::ModelException::ModelException(std::string_view msg) : std::runtime_error(std::string(msg)) {}

tmdl::ExecutionException::ExecutionException(std::string_view msg, const size_t id) : ModelException(msg), _id(id) {}

size_t tmdl::ExecutionException::get_id() const { return _id; }
