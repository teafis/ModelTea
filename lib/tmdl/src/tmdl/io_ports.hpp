// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_IO_PORTS_HPP
#define TF_MODEL_IO_PORTS_HPP

#include <tmdl/block_interface.hpp>

#include <tmdl/model_exception.hpp>

namespace tmdl
{

class InputPort : public BlockInterface
{
public:
    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_port(
        const size_t port,
        const PortValue* value) override;

    const PortValue* get_output_port(const size_t port) const override;

    //BlockExecutionInterface* get_executor() const override;

public:
    void set_input_value(const PortValue* value);

protected:
    PortValue _port;
    std::unique_ptr<BlockExecutionInterface> _executor;
};

class OutputPort : public BlockInterface
{
public:
    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_port(
        const size_t port,
        const PortValue* value) override;

    const PortValue* get_output_port(const size_t port) const override;

    //BlockExecutionInterface* get_executor() const override;

public:
    const PortValue* get_output_value() const;

protected:
    const PortValue* _port;
    std::unique_ptr<BlockExecutionInterface> _executor;
};

}

#endif // TF_MODEL_IO_PORTS_HPP
