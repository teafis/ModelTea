// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VARIABLE_MANAGER_HPP
#define TF_MODEL_VARIABLE_MANAGER_HPP

#include "connection.hpp"
#include "values/value.hpp"

#include <unordered_map>

namespace tmdl
{

struct VariableIdentifier
{
    size_t block_id;
    size_t output_port_num;

    bool operator==(const VariableIdentifier& other) const = default;

    std::string to_string() const;
};

}

namespace std
{

template<>
struct hash<tmdl::VariableIdentifier>
{
    size_t operator()(const tmdl::VariableIdentifier& x) const
    {
        return x.block_id ^ x.output_port_num;
    }
};

}

namespace tmdl
{

class VariableManager
{
public:
    void add_variable(
        const VariableIdentifier id,
        const std::shared_ptr<ModelValue> value);

    std::shared_ptr<ModelValue> get_ptr(const VariableIdentifier& id) const;

    std::shared_ptr<ModelValue> get_ptr(const Connection& c) const;

    bool has_variable(const VariableIdentifier& id) const;

    bool has_variable(const Connection& c) const;

private:
    std::unordered_map<VariableIdentifier, std::shared_ptr<ModelValue>> variables;
};

}

#endif // TF_MODEL_VARIABLE_MANAGER_HPP
