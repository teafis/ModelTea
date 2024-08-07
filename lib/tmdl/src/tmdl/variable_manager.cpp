// SPDX-License-Identifier: GPL-3.0-only

module;

#include <unordered_map>

export module tmdl:variable_manager;

import :connection;
import :model_exception;
import :values;

namespace tmdl {

struct VariableIdentifier {
    size_t block_id;
    size_t output_port_num;

    bool operator==(const VariableIdentifier& other) const = default;

    std::string to_string() const;
};

}

namespace std {

template <> struct hash<tmdl::VariableIdentifier> {
    size_t operator()(const tmdl::VariableIdentifier& x) const { return x.block_id ^ x.output_port_num; }
};

}

namespace tmdl {

class VariableManager {
public:
    void add_variable(const VariableIdentifier id, const std::shared_ptr<ModelValue> value);

    std::shared_ptr<ModelValue> get_ptr(const VariableIdentifier& id) const;

    std::shared_ptr<ModelValue> get_ptr(const Connection& c) const;

    bool has_variable(const VariableIdentifier& id) const;

    bool has_variable(const Connection& c) const;

private:
    std::unordered_map<VariableIdentifier, std::shared_ptr<ModelValue>> variables;
};

}

std::string tmdl::VariableIdentifier::to_string() const { return fmt::format("{}:{}", block_id, output_port_num); }

static tmdl::VariableIdentifier connection_to_variable_id(const tmdl::Connection& c) {
    return tmdl::VariableIdentifier{.block_id = c.get_from_id(), .output_port_num = c.get_from_port()};
}

void tmdl::VariableManager::add_variable(const VariableIdentifier id, const std::shared_ptr<ModelValue> value) {
    const auto it = variables.find(id);

    if (it != variables.end()) {
        throw ModelException("variable with provided name already exists");
    } else if (value == nullptr) {
        throw ModelException("cannot add a null pointer to the variables list");
    } else {
        variables.try_emplace(id, value);
    }
}

std::shared_ptr<tmdl::ModelValue> tmdl::VariableManager::get_ptr(const VariableIdentifier& id) const {
    const auto it = variables.find(id);
    if (it != variables.end()) {
        return it->second;
    } else {
        throw ModelException("variable with identifier not found");
    }
}

std::shared_ptr<tmdl::ModelValue> tmdl::VariableManager::get_ptr(const Connection& c) const {
    return get_ptr(connection_to_variable_id(c));
}

bool tmdl::VariableManager::has_variable(const VariableIdentifier& id) const { return variables.contains(id); }

bool tmdl::VariableManager::has_variable(const Connection& c) const { return has_variable(connection_to_variable_id(c)); }
