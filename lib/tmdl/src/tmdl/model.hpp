// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_HPP
#define TF_MODEL_HPP

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "block_interface.hpp"
#include "connection_manager.hpp"

#include <nlohmann/json.hpp>

namespace tmdl
{

class ModelExecutionInterface : public BlockExecutionInterface
{
public:
    virtual std::shared_ptr<const VariableManager> get_variable_manager() const = 0;
};

class Model;

void to_json(nlohmann::json& j, const Model& m);
void from_json(const nlohmann::json& j, Model& m);

class Model
{
public:
    friend class ModelBlock;

    Model(const std::string& name);

public:
    void add_block(const std::shared_ptr<BlockInterface> block);

    void add_block(const std::shared_ptr<BlockInterface> block, const size_t id);

    void remove_block(const size_t id);

    void add_connection(const std::shared_ptr<Connection> connection);

    void remove_connection(const size_t to_block, const size_t to_port);

public:
    std::string get_name() const;

    void set_name(const std::string& s);

    std::string get_description() const;

    void set_description(const std::string& s);

    double get_preferred_dt() const;

    void set_preferred_dt(const double dt);

    size_t get_num_inputs() const;

    size_t get_num_outputs() const;

    const std::vector<size_t>& get_input_ids() const;

    const std::vector<size_t>& get_output_ids() const;

    DataType get_input_datatype(const size_t port) const;

    DataType get_output_datatype(const size_t port) const;

    bool update_block();

    std::unique_ptr<const BlockError> has_error() const;

protected:
    std::unique_ptr<const BlockError> own_error() const;

public:
    std::shared_ptr<ModelExecutionInterface> get_execution_interface(
        const size_t block_id,
        const ConnectionManager& connections,
        const VariableManager& manager) const;

public:
    std::vector<std::unique_ptr<const BlockError>> get_all_errors() const;

    const ConnectionManager& get_connection_manager() const;

protected:
    size_t get_next_id() const;

public:
    std::shared_ptr<BlockInterface> get_block(const size_t id) const;

    std::vector<std::shared_ptr<BlockInterface>> get_blocks() const;

    bool contains_model_name(const std::string& name) const;

protected:
    std::string name;
    std::string description;
    std::unordered_map<size_t, std::shared_ptr<BlockInterface>> blocks;
    ConnectionManager connections;
    std::vector<size_t> input_ids;
    std::vector<size_t> output_ids;
    double preferred_dt;

public:
    friend void to_json(nlohmann::json&, const Model&);

    friend void from_json(const nlohmann::json&, Model&);
};

}

#endif // TF_MODEL_HPP
