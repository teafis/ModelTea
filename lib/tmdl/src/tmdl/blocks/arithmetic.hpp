// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_ARITHMETIC_HPP
#define TF_MODEL_STDLIB_ARITHMETIC_HPP

#include "../library.hpp"

namespace tmdl::stdlib
{

class ArithmeticBase : public LibraryBlock
{
public:
    ArithmeticBase();

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    std::vector<Parameter*> get_parameters() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_port(
        const size_t port,
        const PortValue value) override;

    PortValue get_output_port(const size_t port) const override;

private:
    size_t currentPrmPortCount() const;

protected:
    std::unique_ptr<Parameter> _prmNumInputPorts;
    std::vector<PortValue> _inputPorts;
    PortValue _outputPort;
};

class Addition : public ArithmeticBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;
};

class Subtraction : public ArithmeticBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;
};

class Product : public ArithmeticBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;
};

class Divide : public ArithmeticBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;
};

}

#endif // TF_MODEL_STDLIB_ARITHMETIC_HPP
