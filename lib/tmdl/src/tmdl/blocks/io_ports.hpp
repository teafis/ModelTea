// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_IO_PORTS_HPP
#define TF_MODEL_IO_PORTS_HPP

#include "../library.hpp"

namespace tmdl
{

class InputPort : public LibraryBlock
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::vector<Parameter*> get_parameters() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_port(
        const size_t port,
        const PortValue value) override;

    PortValue get_output_port(const size_t port) const override;

    std::unique_ptr<const BlockError> has_error() const override;

    bool update_block() override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;

public:
    void set_input_value(const PortValue value);

protected:
    PortValue _port;
};

class OutputPort : public LibraryBlock
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::vector<Parameter*> get_parameters() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_port(
        const size_t port,
        const PortValue value) override;

    PortValue get_output_port(const size_t port) const override;

    std::unique_ptr<const BlockError> has_error() const override;

    bool update_block() override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;

public:
    PortValue get_output_value() const;

protected:
    PortValue _port;
};

}

#endif // TF_MODEL_IO_PORTS_HPP
