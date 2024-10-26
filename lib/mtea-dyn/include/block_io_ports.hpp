// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNBLOCKS_IO_PORTS_HPP
#define MTEA_DYNBLOCKS_IO_PORTS_HPP

#include "block_interface.hpp"

namespace mtea {

class InputPort final : public BlockInterface {
public:
    InputPort(std::string_view library);

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_type(const size_t port, const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::unique_ptr<const BlockError> has_error() const override;

    std::vector<std::shared_ptr<Parameter>> get_parameters() const override;

    bool update_block() override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;

    void set_input_value(const DataType type);

private:
    DataType get_output_type() const;

    DataType _port;
    std::shared_ptr<ParameterDataType> dataTypeParameter;
};

class OutputPort final : public BlockInterface {
public:
    OutputPort(std::string_view library);

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_type(const size_t port, const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::unique_ptr<const BlockError> has_error() const override;

    bool update_block() override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;

    DataType get_output_value() const;

private:
    DataType _port;
};

}

#endif // MTEA_DYNBLOCKS_IO_PORTS_HPP
