// SPDX-License-Identifier: GPL-3.0-only

#include "variable_manager.hpp"

#include "model_exception.hpp"

#include <fmt/format.h>

std::string mtea::VariableIdentifier::to_string() const { return fmt::format("{}:{}", block_id, output_port_num); }

static mtea::VariableIdentifier connection_to_variable_id(const mtea::Connection& c) {
    return mtea::VariableIdentifier{.block_id = c.get_from_id(), .output_port_num = c.get_from_port()};
}

void mtea::VariableManager::add_variable(const VariableIdentifier id, const std::shared_ptr<ModelValue> value) {
    const auto it = variables.find(id);

    if (it != variables.end()) {
        throw ModelException("variable with provided name already exists");
    } else if (value == nullptr) {
        throw ModelException("cannot add a null pointer to the variables list");
    } else {
        variables.try_emplace(id, value);
    }
}

std::shared_ptr<mtea::ModelValue> mtea::VariableManager::get_ptr(const VariableIdentifier& id) const {
    const auto it = variables.find(id);
    if (it != variables.end()) {
        return it->second;
    } else {
        throw ModelException("variable with identifier not found");
    }
}

std::shared_ptr<mtea::ModelValue> mtea::VariableManager::get_ptr(const Connection& c) const {
    return get_ptr(connection_to_variable_id(c));
}

bool mtea::VariableManager::has_variable(const VariableIdentifier& id) const { return variables.contains(id); }

bool mtea::VariableManager::has_variable(const Connection& c) const { return has_variable(connection_to_variable_id(c)); }
