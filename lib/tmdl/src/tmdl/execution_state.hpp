// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_EXECUTION_STATE_HPP
#define TF_MODEL_EXECUTION_STATE_HPP

#include <cstdint>

#include <memory>
#include <string>
#include <unordered_map>

#include "block_interface.hpp"
#include "model.hpp"
#include "variable_manager.hpp"

namespace tmdl
{

struct ExecutionState
{
    std::shared_ptr<BlockExecutionInterface> model;
    std::shared_ptr<VariableManager> variables;
    std::unordered_map<std::string, std::shared_ptr<const ModelValue>> named_variables;
    SimState state;
    uint64_t iterations;

    void reset();

    std::vector<std::string> get_variable_names() const;

    static ExecutionState from_model(
        const std::shared_ptr<Model> model,
        const double dt);
};

}

#endif // TF_MODEL_EXECUTION_STATE_HPP
