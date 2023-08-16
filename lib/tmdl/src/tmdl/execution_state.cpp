// SPDX-License-Identifier: GPL-3.0-only

#include "execution_state.hpp"

#include <algorithm>

#include <fmt/format.h>

#include "model_exception.hpp"


tmdl::ExecutionState::ExecutionState(
    std::shared_ptr<BlockExecutionInterface> model,
    std::shared_ptr<VariableManager> variables,
    const double dt) :
    model{ model },
    variables{ variables },
    state{ dt },
    iterations{ 0 }
{
    // Empty Constructor
}

tmdl::ExecutionState::~ExecutionState()
{
    model->close();
}

void tmdl::ExecutionState::init()
{
    model->init(state);
}

void tmdl::ExecutionState::step()
{
    iterations += 1;
    model->step(state);
}

void tmdl::ExecutionState::reset()
{
    iterations = 0;
    model->reset(state);
}

double tmdl::ExecutionState::get_current_time() const
{
    return iterations * state.get_dt();
}

std::shared_ptr<const tmdl::BlockExecutionInterface> tmdl::ExecutionState::get_model() const
{
    return model;
}

uint64_t tmdl::ExecutionState::get_iterations() const
{
    return iterations;
}

std::shared_ptr<const tmdl::VariableManager> tmdl::ExecutionState::get_variable_manager() const
{
    return variables;
}

std::vector<std::string> tmdl::ExecutionState::get_variable_names() const
{
    std::vector<std::string> names;
    for (const auto& it : named_variables)
    {
        names.push_back(it.first);
    }
    std::sort(names.begin(), names.end());
    return names;
}

std::shared_ptr<const tmdl::ModelValue> tmdl::ExecutionState::get_variable_for_name(const std::string& n) const
{
    const auto it = named_variables.find(n);
    if (it != named_variables.end())
    {
        return it->second;
    }
    else
    {
        throw tmdl::ModelException(fmt::format("unable to find named variable with name '{}'", n));
    }
}

void tmdl::ExecutionState::add_name_to_variable(const std::string& name, const VariableIdentifier id)
{
    add_name_to_variable(name, variables->get_ptr(id));
}

void tmdl::ExecutionState::add_name_to_variable(const std::string& name, const Connection conn)
{
    add_name_to_variable(name, variables->get_ptr(conn));
}

void tmdl::ExecutionState::add_name_to_interior_variable(const std::string& name, VariableIdentifier id)
{
    add_name_to_variable(name, get_model_exec_interface()->get_variable_manager()->get_ptr(id));
}

void tmdl::ExecutionState::add_name_to_interior_variable(const std::string& name, Connection conn)
{
    add_name_to_variable(name, get_model_exec_interface()->get_variable_manager()->get_ptr(conn));
}

std::shared_ptr<const tmdl::ModelExecutionInterface> tmdl::ExecutionState::get_model_exec_interface() const
{
    auto model_exec = std::dynamic_pointer_cast<tmdl::ModelExecutionInterface>(model);
    if (model_exec == nullptr)
    {
        throw ModelException("unable to find model execution interface");
    }

    return model_exec;
}

void tmdl::ExecutionState::add_name_to_variable(const std::string& name, std::shared_ptr<const ModelValue> variable)
{
    const auto it = named_variables.find(name);
    if (it != named_variables.end())
    {
        throw tmdl::ModelException(fmt::format("name '{}' already exists for variable", name));
    }

    named_variables[name] = variable;
}

tmdl::ExecutionState tmdl::ExecutionState::from_model(
    const std::shared_ptr<Model> model,
    const double dt)
{
    // Parameter generator
    tmdl::ConnectionManager connections;
    std::shared_ptr<VariableManager> manager = std::make_shared<VariableManager>();

    // Add each output variable to the manager
    for (size_t i = 0; i < model->get_num_outputs(); ++i)
    {
        const auto pv = model->get_output_datatype(i);

        const std::shared_ptr<ModelValue> value = std::shared_ptr<ModelValue>(ModelValue::make_default_type(pv));

        const auto vid = VariableIdentifier
        {
            .block_id = 0,
            .output_port_num = i
        };

        manager->add_variable(vid, value);
    }

    // Ensure that the block is updated
    model->update_block(); // TODO - ERROR IF NOT UPDATED?

    // Construct and return the executor
    auto exec_state = tmdl::ExecutionState(
        model->get_execution_interface(0, connections, *manager),
        manager,
        dt);

    return exec_state;
}
