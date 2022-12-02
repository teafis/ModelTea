// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCKS_ARITHMETIC_HPP
#define TF_MODEL_BLOCKS_ARITHMETIC_HPP

#include "../block_interface.hpp"

namespace tmdl::stdlib
{

class ArithmeticBase : public BlockInterface
{
public:
    struct FunctionTypes
    {
        double (*double_fcn)(const double&, const double&);
        float (*float_fcn)(const float&, const float&);
        int32_t (*i32_fcn)(const int32_t&, const int32_t&);
        uint32_t (*u32_fcn)(const uint32_t&, const uint32_t&);
    };

public:
    ArithmeticBase();

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    std::vector<std::shared_ptr<Parameter>> get_parameters() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_type(
        const size_t port,
        const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;

protected:
    virtual FunctionTypes get_application_functions() const = 0;

private:
    size_t currentPrmPortCount() const;

protected:
    std::shared_ptr<Parameter> _prmNumInputPorts;
    std::vector<DataType> _inputTypes;
    DataType _outputPort;
};

class Addition : public ArithmeticBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    FunctionTypes get_application_functions() const override;
};

class Subtraction : public ArithmeticBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    FunctionTypes get_application_functions() const override;
};

class Multiplication : public ArithmeticBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    FunctionTypes get_application_functions() const override;
};

class Division : public ArithmeticBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    FunctionTypes get_application_functions() const override;
};

}

#endif // TF_MODEL_BLOCKS_ARITHMETIC_HPP
