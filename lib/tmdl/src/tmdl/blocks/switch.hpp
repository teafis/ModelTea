// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCKS_SWITCH_HPP
#define TF_MODEL_BLOCKS_SWITCH_HPP

#include "../block_interface.hpp"

namespace tmdl::blocks {

class Switch : public BlockInterface {
public:
    Switch();

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_type(const size_t port, const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;

private:
    DataType input_type_tf;
    DataType input_type_a;
    DataType input_type_b;
    DataType output_port;
};

}

#endif // TF_MODEL_BLOCKS_SWITCH_HPP
