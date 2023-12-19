// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCKS_CONSTANT_HPP
#define TF_MODEL_BLOCKS_CONSTANT_HPP

#include "../block_interface.hpp"

namespace tmdl::blocks {

class Constant : public BlockInterface {
public:
    Constant();

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    std::vector<std::shared_ptr<Parameter>> get_parameters() const override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_type(const size_t, const DataType) override;

    DataType get_output_type(const size_t port) const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;

private:
    DataType get_data_type() const;

    std::shared_ptr<Parameter> param_dtype;
    std::shared_ptr<Parameter> param_value;
    DataType output_port;
};

}

#endif // TF_MODEL_BLOCKS_CONSTANT_HPP
