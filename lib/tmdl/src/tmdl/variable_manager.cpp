// SPDX-License-Identifier: GPL-3.0-only

#include "variable_manager.hpp"

#include "model_exception.hpp"


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
    const VariableIdentifier vid = VariableIdentifier
    {
        .block_id = c.get_from_id(),
        .output_port_num = c.get_from_port()
    };

    return get_ptr(vid);
}
