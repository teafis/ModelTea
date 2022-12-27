// SPDX-License-Identifier: GPL-3.0-only

#include "variable_manager.hpp"

#include "model_exception.hpp"

#include <fmt/format.h>

std::string tmdl::VariableIdentifier::to_string() const
{
    return fmt::format("{}:{}", block_id, output_port_num);
}


static tmdl::VariableIdentifier connection_to_variable_id(const tmdl::Connection& c)
{
    return tmdl::VariableIdentifier
    {
        .block_id = c.get_from_id(),
        .output_port_num = c.get_from_port()
    };
}


void tmdl::VariableManager::add_variable(const VariableIdentifier id, const std::shared_ptr<ValueBox> value)
{
    const auto it = variables.find(id);

    if (it != variables.end())
    {
        throw ModelException("variable with provided name already exists");
    }
    else if (value == nullptr)
    {
        throw ModelException("cannot add a null pointer to the variables list");
    }
    else
    {
        variables.insert({ id, value });
    }
}

std::shared_ptr<tmdl::ValueBox> tmdl::VariableManager::get_ptr(const VariableIdentifier& id) const
{
    const auto it = variables.find(id);
    if (it != variables.end())
    {
        return it->second;
    }
    else
    {
        throw ModelException("variable with identifier not found");
    }
}

std::shared_ptr<tmdl::ValueBox> tmdl::VariableManager::get_ptr(const Connection& c) const
{
    return get_ptr(connection_to_variable_id(c));
}

std::shared_ptr<tmdl::ValueBox> tmdl::VariableManager::get_ptr(const std::string& name) const
{
    return get_ptr(get_identifier(name));
}

bool tmdl::VariableManager::has_variable(const VariableIdentifier& id) const
{
    return variables.find(id) != variables.end();
}

bool tmdl::VariableManager::has_variable(const Connection& c) const
{
    return has_variable(connection_to_variable_id(c));
}

bool tmdl::VariableManager::has_variable(const std::string& n) const
{
    const auto it = tagged_names.find(n);
    return it != tagged_names.end() && has_variable(it->second);
}

void tmdl::VariableManager::set_name_for_variable(const std::string& name, const VariableIdentifier& id)
{
    if (variables.find(id) == variables.end())
    {
        throw ModelException(fmt::format("cannot assign '{}' to ID '{}' - it does not exist in the current map", name, id.to_string()));
    }

    tagged_names.insert({name, id});
}

void tmdl::VariableManager::set_name_for_variable(const std::string& name, const Connection& conn)
{
    set_name_for_variable(name, connection_to_variable_id(conn));
}

void tmdl::VariableManager::clear_name_for_variable(const std::string& name)
{
    const auto it = tagged_names.find(name);
    if (it != tagged_names.end())
    {
        tagged_names.erase(it);
    }
}

std::vector<std::string> tmdl::VariableManager::get_names() const
{
    std::vector<std::string> names;
    for (const auto& it : tagged_names)
    {
        names.push_back(it.first);
    }
    return names;
}

tmdl::VariableIdentifier tmdl::VariableManager::get_identifier(const std::string& name) const
{
    const auto it = tagged_names.find(name);
    if (it != tagged_names.end())
    {
        return it->second;
    }
    else
    {
        throw ModelException(fmt::format("variable identifier with name '{}' not found", name));
    }
}
