// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_TRIG_HPP
#define TF_MODEL_STDLIB_TRIG_HPP

#include "../block_interface.hpp"

#include <cmath>

namespace tmdl::stdlib
{

class TrigFunction : public BlockInterface
{
public:
    TrigFunction();

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_port(
        const size_t port,
        const DataType type) override;

    PortValue get_output_port(const size_t port) const override;

protected:
    DataType input_type;
    PortValue output_port;
};

class TrigSin : public TrigFunction
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;
};

class TrigCos : public TrigFunction
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;
};

}

#endif // TF_MODEL_STDLIB_TRIG_HPP
