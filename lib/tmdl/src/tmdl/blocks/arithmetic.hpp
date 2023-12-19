// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCKS_ARITHMETIC_HPP
#define TF_MODEL_BLOCKS_ARITHMETIC_HPP

#include "../block_interface.hpp"

namespace tmdl::blocks {

class ArithmeticBase : public BlockInterface {
public:
    ArithmeticBase();

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    std::vector<std::shared_ptr<Parameter>> get_parameters() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_type(const size_t port, const DataType type) override;

    DataType get_output_type(const size_t port) const override;

protected:
    DataType get_output_type() const;

private:
    size_t currentPrmPortCount() const;

    std::shared_ptr<Parameter> _prmNumInputPorts;
    std::vector<DataType> _inputTypes;
    DataType _outputPort;
};

class Addition : public ArithmeticBase {
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

class Subtraction : public ArithmeticBase {
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

class Multiplication : public ArithmeticBase {
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

class Division : public ArithmeticBase {
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

class Modulus : public ArithmeticBase {
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

}

#endif // TF_MODEL_BLOCKS_ARITHMETIC_HPP
