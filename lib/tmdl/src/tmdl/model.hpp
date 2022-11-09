// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_HPP
#define TF_MODEL_HPP

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>

#include <tmdl/block.hpp>
#include <tmdl/block_interface.hpp>
#include <tmdl/connection.hpp>
#include <tmdl/io_ports.hpp>
#include <tmdl/model_exception.hpp>


namespace tmdl
{

class Model : public BlockInterface
{
public:
    void add_block(const std::shared_ptr<BlockInterface> block);

    void remove_block(const size_t id);

    void add_connection(Connection connection);

    void remove_connection(const size_t to_block, const size_t to_port);

public:
    size_t get_num_outputs() const override;

    size_t get_num_inputs() const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override;

public:
    class ModelExecutor : public BlockExecutionInterface
    {
    public:
        ModelExecutor(
            const Model* parent,
            const std::vector<std::shared_ptr<BlockExecutionInterface>>& blocks);

        void step() override;

        void reset() override;

    protected:
        std::vector<PortValue*> input_blocks;
        std::vector<const PortValue*> output_blocks;
        std::vector<BlockExecutionInterface*> blocks;
    };

protected:
    size_t get_next_id() const;

    BlockInterface* get_block(const size_t id) const;

protected:
    std::unordered_map<size_t, std::shared_ptr<BlockInterface>> blocks;
    std::vector<std::unique_ptr<Connection>> connections;
    std::vector<size_t> input_ids;
    std::vector<size_t> output_ids;
};

}

#endif // TF_MODEL_HPP
