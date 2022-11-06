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
    virtual size_t get_num_inputs() const override
    {
        return 0;
    }

    virtual size_t get_num_outputs() const override
    {
        return 1;
    }

    virtual void set_input_value(const size_t port, std::unique_ptr<Value> value) override
    {
        if (port == 0)
        {
            this->value = std::move(value);
        }
        else
        {
            throw ModelException("unable to set input value");
        }
    }

    virtual std::unique_ptr<Value> get_output_value(const size_t port) const override
    {
        if (port == 0)
        {
            return value->clone();
        }
        else
        {
            return nullptr;
        }
    }

    virtual void step() override
    {
        // Empty Block
    }

    virtual void reset() override
    {
        // Empty Block
    }

protected:
    std::unique_ptr<Value> value;
};

class OutputPort : public BlockInterface
{
public:
    virtual size_t get_num_inputs() const override
    {
        return 1;
    }

    virtual size_t get_num_outputs() const override
    {
        return 0;
    }

    virtual void set_input_value(const size_t port, std::unique_ptr<Value> value) override
    {
        if (port == 0)
        {
            this->value = std::move(value);
        }
        else
        {
            throw ModelException("unable to set input port value");
        }
    }

    virtual std::unique_ptr<Value> get_output_value(const size_t) const override
    {
        throw ModelException("No output value for OutputPort");
    }

    virtual void step() override
    {
        // Empty Block
    }

    virtual void reset() override
    {
        // Empty Block
    }

public:
    const std::unique_ptr<Value>& get_value() const
    {
        return value;
    }

protected:
    std::unique_ptr<Value> value;
};

}

#endif // TF_MODEL_IO_PORTS_HPP
