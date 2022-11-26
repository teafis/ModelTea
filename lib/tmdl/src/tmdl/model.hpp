// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_HPP
#define TF_MODEL_HPP

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>

#include "library.hpp"
#include "block_interface.hpp"
#include "connection_manager.hpp"

#include "nlohmann/json.hpp"


namespace tmdl
{

class Model : public BlockInterface
{
public:
    void add_block(const std::shared_ptr<BlockInterface> block);

    void add_block(const std::shared_ptr<BlockInterface> block, const size_t id);

    void remove_block(const size_t id);

    void add_connection(Connection connection);

    void remove_connection(const size_t to_block, const size_t to_port);

public:
    size_t get_num_outputs() const override;

    size_t get_num_inputs() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_port(
        const size_t port,
        const DataType type) override;

    PortValue get_output_port(const size_t port) const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;

public:
    std::vector<std::unique_ptr<const BlockError>> get_all_errors() const;

    const ConnectionManager& get_connection_manager() const;

protected:
    size_t get_next_id() const;

public:
    std::shared_ptr<BlockInterface> get_block(const size_t id) const;

    const std::vector<size_t>& get_output_ids() const
    {
        return output_ids;
    }

    const std::vector<size_t>& get_input_ids() const
    {
        return input_ids;
    }

    std::vector<std::shared_ptr<const BlockInterface>> get_all_blocks() const
    {
        std::vector<std::shared_ptr<const BlockInterface>> blks;
        for (const auto& blk : blocks)
        {
            blks.push_back(blk.second);
        }
        return blks;
    }

protected:
    std::unordered_map<size_t, std::shared_ptr<BlockInterface>> blocks;
    ConnectionManager connections;
    std::vector<size_t> input_ids;
    std::vector<size_t> output_ids;
};

}

namespace ns
{

void to_json(nlohmann::json& j, const tmdl::Model& m);

void from_json(const nlohmann::json& j, tmdl::Model& m);

}

#endif // TF_MODEL_HPP
