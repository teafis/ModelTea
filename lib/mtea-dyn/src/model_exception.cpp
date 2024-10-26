// SPDX-License-Identifier: GPL-3.0-only

#include "model_exception.hpp"

mtea::ModelException::ModelException(std::string_view msg) : std::runtime_error(std::string(msg)) {}

mtea::ModelException::ModelException(const mtea::block_error& err) : ModelException(std::string_view(err.what())) {}

mtea::ExecutionException::ExecutionException(std::string_view msg, const size_t id) : ModelException(msg), _id(id) {}

size_t mtea::ExecutionException::get_id() const { return _id; }
