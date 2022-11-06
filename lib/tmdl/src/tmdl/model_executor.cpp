// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/model.hpp>

using namespace tmdl;

Model::ModelExecutor::ModelExecutor(const Model* parent) :
    BlockExecutionInterface(parent)
{
    // Empty Constructor
}

void Model::ModelExecutor::set_input_value(
    const size_t port,
    std::shared_ptr<const Value> value)
{
    if (port < input_values.size())
    {
        input_values[port] = value;
    }
}

std::shared_ptr<const Value> Model::ModelExecutor::get_output_value(const size_t port) const
{
    if (port < output_values.size())
    {
        return output_values[port];
    }
    else
    {
        throw ModelException("requested port exceeds model size");
    }
}

void Model::ModelExecutor::step()
{
    // Set input parameters


    // Step each block (already in execution order)
    for (const auto& b : blocks)
    {
        b->step();
    }
}

void Model::ModelExecutor::reset()
{
    for (const auto& b : blocks)
    {
        b->reset();
    }
}
