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

class ExecutionState
{
public:
    ExecutionState(
        std::shared_ptr<BlockExecutionInterface> model,
        std::shared_ptr<VariableManager> variables,
        const double dt);

    ~ExecutionState();

    void init();

    void step();

    void reset();

    double get_current_time() const;

    std::shared_ptr<const BlockExecutionInterface> get_model() const;

    uint64_t get_iterations() const;

    std::shared_ptr<const VariableManager> get_variable_manager() const;

    std::vector<std::string> get_variable_names() const;

    std::shared_ptr<const ModelValue> get_variable_for_name(const std::string& n) const;

    void add_name_to_variable(const std::string& name, VariableIdentifier id);

    void add_name_to_variable(const std::string& name, Connection conn);

    void add_name_to_interior_variable(const std::string& name, VariableIdentifier id);

    void add_name_to_interior_variable(const std::string& name, Connection conn);

    static ExecutionState from_model(
        const std::shared_ptr<Model> model,
        const double dt);

protected:
    std::shared_ptr<const ModelExecutionInterface> get_model_exec_interface() const;

    void add_name_to_variable(const std::string& name, std::shared_ptr<const ModelValue> variable);

protected:
    std::shared_ptr<BlockExecutionInterface> model;
    std::shared_ptr<VariableManager> variables;
    std::unordered_map<std::string, std::shared_ptr<const ModelValue>> named_variables;
    BlockInterface::ModelInfo state;
    uint64_t iterations;
};

}

#endif // TF_MODEL_EXECUTION_STATE_HPP
