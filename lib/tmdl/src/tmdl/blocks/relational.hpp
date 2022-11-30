// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_RELATIONAL_HPP
#define TF_MODEL_STDLIB_RELATIONAL_HPP

#include "../block_interface.hpp"

namespace tmdl::stdlib
{

class RelationalBase : public BlockInterface
{
public:
    struct FunctionTypes
    {
        bool (*double_fcn)(const double&, const double&);
        bool (*float_fcn)(const float&, const float&);
        bool (*i32_fcn)(const int32_t&, const int32_t&);
        bool (*u32_fcn)(const uint32_t&, const uint32_t&);
        bool (*bool_fcn)(const bool&, const bool&);
    };

public:
    RelationalBase();

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_port(
        const size_t port,
        const DataType type) override;

    PortValue get_output_port(const size_t port) const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;

protected:
    virtual FunctionTypes get_application_functions() const = 0;

protected:
    DataType _inputA;
    DataType _inputB;
    PortValue _outputPort;
};

class GreaterThan : public RelationalBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    FunctionTypes get_application_functions() const override;
};

class GreaterThanEqual : public RelationalBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    FunctionTypes get_application_functions() const override;
};

class LessThan : public RelationalBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    FunctionTypes get_application_functions() const override;
};

class LessThanEqual : public RelationalBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    FunctionTypes get_application_functions() const override;
};

class Equal : public RelationalBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    FunctionTypes get_application_functions() const override;
};

class NotEqual : public RelationalBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    FunctionTypes get_application_functions() const override;
};

}

#endif // TF_MODEL_STDLIB_RELATIONAL_HPP