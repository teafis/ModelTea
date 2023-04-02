// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCKS_IO_PORTS_HPP
#define TF_MODEL_BLOCKS_IO_PORTS_HPP

#include "../block_interface.hpp"

namespace tmdl
{

class InputPort : public CodegenBlockInterface
{
public:
    InputPort();

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_type(
        const size_t port,
        const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::unique_ptr<const BlockError> has_error() const override;

    std::vector<std::shared_ptr<Parameter>> get_parameters() const override;

    bool update_block() override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_component() const override;

public:
    void set_input_value(const DataType type);

protected:
    DataType _port;
    std::shared_ptr<Parameter> dataTypeParameter;
};

class OutputPort : public CodegenBlockInterface
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_type(
        const size_t port,
        const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::unique_ptr<const BlockError> has_error() const override;

    bool update_block() override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_component() const override;

public:
    DataType get_output_value() const;

protected:
    DataType _port;
};

}

#endif // TF_MODEL_BLOCKS_IO_PORTS_HPP
