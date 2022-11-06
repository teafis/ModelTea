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
    void add_block(std::unique_ptr<BlockInterface> block);


    void remove_block(const size_t id);

    void add_connection(const Connection connection);

    void remove_connection(const size_t to_block, const size_t to_port);

public:
    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_value(
        const size_t port,
        std::unique_ptr<Value> value) override;

    std::unique_ptr<Value> get_output_value(const size_t port) const override;

    void step() override;

    void reset() override;

protected:
    void compile_execution_order();

    size_t get_next_id() const;

    BlockInterface* get_block(const size_t id) const;

protected:
    std::optional<std::vector<size_t>> execution_order;
    std::unordered_map<size_t, std::unique_ptr<BlockInterface>> blocks;
    std::vector<Connection> connections;
    std::vector<size_t> input_ids;
    std::vector<size_t> output_ids;
};

}

#endif // TF_MODEL_HPP
