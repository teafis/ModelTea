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

    virtual std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override;

public:
    class Executor : public BlockExecutionInterface
    {
    public:
        Executor(const InputPort* parent);

        void set_value(std::shared_ptr<const Value> value);

        virtual void set_input_value(
            const size_t,
            std::shared_ptr<const Value>) override;

        virtual std::shared_ptr<const Value> get_output_value(const size_t port) const override;

    protected:
        std::shared_ptr<const Value> _value;
    };
};

class OutputPort : public BlockInterface
{
public:
    virtual size_t get_num_inputs() const override;

    virtual size_t get_num_outputs() const override;

    virtual std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override;

public:
    class Executor : public BlockExecutionInterface
    {
    public:
        Executor(const OutputPort* parent);

        virtual void set_input_value(
            const size_t port,
            const std::shared_ptr<const Value> value) override;

        virtual std::shared_ptr<const Value> get_output_value(const size_t) const override;
        std::shared_ptr<const Value> get_value() const;

    protected:
        std::shared_ptr<const Value> _value;
    };
};

}

#endif // TF_MODEL_IO_PORTS_HPP
