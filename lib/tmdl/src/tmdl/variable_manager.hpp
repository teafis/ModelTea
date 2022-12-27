// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VARIABLE_MANAGER_HPP
#define TF_MODEL_VARIABLE_MANAGER_HPP

#include "connection.hpp"
#include "value.hpp"

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
        const std::shared_ptr<ValueBox> value);

    std::shared_ptr<ValueBox> get_ptr(const VariableIdentifier& id) const;

    std::shared_ptr<ValueBox> get_ptr(const Connection& c) const;

    std::shared_ptr<ValueBox> get_ptr(const std::string& name) const;

    bool has_variable(const VariableIdentifier& id) const;

    bool has_variable(const Connection& c) const;

    bool has_variable(const std::string& n) const;

    void set_name_for_variable(const std::string& name, const VariableIdentifier& id);

    void set_name_for_variable(const std::string& name, const Connection& conn);

    void clear_name_for_variable(const std::string& name);

    std::vector<std::string> get_names() const;

    VariableIdentifier get_identifier(const std::string& name) const;

protected:
    std::unordered_map<VariableIdentifier, std::shared_ptr<ValueBox>> variables;
    std::unordered_map<std::string, VariableIdentifier> tagged_names;
};

}

#endif // TF_MODEL_VARIABLE_MANAGER_HPP
