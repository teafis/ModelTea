// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCKS_DERIVATIVE_HPP
#define TF_MODEL_BLOCKS_DERIVATIVE_HPP

#include "../block_interface.hpp"

namespace tmdl::blocks
{

class Derivative : public CodegenBlockInterface
{
public:
    Derivative();

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_type(
        const size_t port,
        const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;

    std::unique_ptr<codegen::CodeComponent> get_codegen_component() const override;

protected:
    DataType input_type;
    DataType input_reset_flag_type;
    DataType output_port;
};

}

#endif // TF_MODEL_BLOCKS_DERIVATIVE_HPP
