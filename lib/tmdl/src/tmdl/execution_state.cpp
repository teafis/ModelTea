// SPDX-License-Identifier: GPL-3.0-only

#include "execution_state.hpp"

#include <algorithm>


void tmdl::ExecutionState::reset()
{
    iterations = 0;
    model->reset();
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

tmdl::ExecutionState tmdl::ExecutionState::from_model(
    const std::shared_ptr<Model> model,
    const double dt)
{
    // Parameter generator
    tmdl::ConnectionManager connections;
    std::shared_ptr<tmdl::VariableManager> manager = std::make_shared<tmdl::VariableManager>();

    // Add each output variable to the manager
    for (size_t i = 0; i < model->get_num_outputs(); ++i)
    {
        const auto pv = model->get_output_datatype(i);

        const std::shared_ptr<tmdl::ModelValue> value = tmdl::make_default_value(pv);

        const auto vid = tmdl::VariableIdentifier
        {
            .block_id = 0,
            .output_port_num = i
        };

        manager->add_variable(vid, value);
    }

    // Ensure that the block is updated
    model->update_block(); // TODO - ERROR IF NOT UPDATED?

    // Construct and return the executor
    return tmdl::ExecutionState
    {
        .model = model->get_execution_interface(0, connections, *manager),
        .variables = manager,
        .named_variables = {},
        .state = tmdl::SimState(dt),
        .iterations = 0
    };
}
