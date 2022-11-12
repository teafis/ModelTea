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

    bool update_block() override;

    void set_input_port(
        const size_t port,
        const PortValue* value) override;

    const PortValue* get_output_port(const size_t port) const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override;

protected:
    size_t get_next_id() const;

    BlockInterface* get_block(const size_t id) const;

protected:
    std::unordered_map<size_t, std::shared_ptr<BlockInterface>> blocks;
    std::vector<Connection> connections;
    std::vector<size_t> input_ids;
    std::vector<size_t> output_ids;
};

}

#endif // TF_MODEL_HPP
