// SPDX-License-Identifier: GPL-3.0-only

#include "execution_state.hpp"

void tmdl::ExecutionState::reset()
{
    iterations = 0;
    state.set_time(0.0);
    model->reset();
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

        const std::shared_ptr<tmdl::ValueBox> value = tmdl::make_shared_default_value(pv);

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
        .model = model->get_execution_interface(connections, *manager),
        .variables = manager,
        .state = tmdl::SimState(dt),
        .iterations = 0
    };
}
