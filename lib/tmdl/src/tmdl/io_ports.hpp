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
    virtual size_t get_num_inputs() const override;

    virtual size_t get_num_outputs() const override;

    virtual void set_input_port(
        const size_t port,
        const PortValue* value) override;

    virtual const PortValue* get_output_port(const size_t port) const override;

    virtual BlockExecutionInterface* get_executor() const override;

public:
    template <typename T>
    class Executor : public BlockExecutionInterface
    {
    public:
        Executor(const InputPort* parent);

        void set_value(const PortValue* value)
        {
            _port = value;
        }

    protected:
        const PortValue* _port;
    };
};

class OutputPort : public BlockInterface
{
public:
    virtual size_t get_num_inputs() const override;

    virtual size_t get_num_outputs() const override;

    virtual BlockExecutionInterface* get_executor() const override;

public:
    template <typename T>
    class Executor : public BlockExecutionInterface
    {
    public:
        Executor(const OutputPort* parent);

        const T get_value() const
        {
            return *reinterpret_cast<const T*>(_port->ptr);
        }

    protected:
        const PortValue* _port;
    };
};

}

#endif // TF_MODEL_IO_PORTS_HPP
