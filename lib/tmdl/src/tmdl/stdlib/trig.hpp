// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_TRIG_HPP
#define TF_MODEL_STDLIB_TRIG_HPP

#include <tmdl/block.hpp>

#include <cmath>

namespace tmdl::stdlib
{

class TrigFunction : public Block
{
public:
    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    void set_input_port(
        const size_t port,
        const PortValue* value) override;

    const PortValue* get_output_port(const size_t port) const override;

protected:
    const PortValue* input_value;

    std::unique_ptr<PortValue> output_port;
    std::unique_ptr<ValueBox> output_port_value;
};

class TrigSin : public TrigFunction
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override;
};

class TrigCos : public TrigFunction
{
public:
    std::string get_name() const override;

    std::string get_description() const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override;
};

}

#endif // TF_MODEL_STDLIB_TRIG_HPP
