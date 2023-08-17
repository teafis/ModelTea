// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCKS_RELATIONAL_HPP
#define TF_MODEL_BLOCKS_RELATIONAL_HPP

#include "../block_interface.hpp"

namespace tmdl::blocks
{

class RelationalBase : public BlockInterface
{
public:
    RelationalBase();

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_type(
        const size_t port,
        const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    virtual std::vector<DataType> get_supported_types() const = 0;

protected:
    DataType _inputA;
    DataType _inputB;
    DataType _outputPort;
};

class RelationalNumericBase : public RelationalBase
{
protected:
    virtual std::vector<DataType> get_supported_types() const override;
};

class RelationalEqualityBase : public RelationalBase
{
protected:
    virtual std::vector<DataType> get_supported_types() const override;
};

class GreaterThan : public RelationalNumericBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

class GreaterThanEqual : public RelationalNumericBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

class LessThan : public RelationalNumericBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

class LessThanEqual : public RelationalNumericBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

class Equal : public RelationalEqualityBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

class NotEqual : public RelationalEqualityBase
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;
};

}

#endif // TF_MODEL_BLOCKS_RELATIONAL_HPP
