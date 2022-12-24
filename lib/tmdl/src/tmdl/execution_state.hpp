// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_EXECUTION_STATE_HPP
#define TF_MODEL_EXECUTION_STATE_HPP

#include <memory>
#include <cstdint>

#include "block_interface.hpp"
#include "model.hpp"
#include "variable_manager.hpp"

namespace tmdl
{

struct ExecutionState
{
    std::shared_ptr<BlockExecutionInterface> model;
    std::shared_ptr<VariableManager> variables;
    SimState state;
    uint64_t iterations;

    void reset();

    static ExecutionState from_model(
        const std::shared_ptr<Model> model,
        const double dt);
};

}

#endif // TF_MODEL_EXECUTION_STATE_HPP
