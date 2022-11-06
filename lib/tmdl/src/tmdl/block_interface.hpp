// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_IO_INTERFACES_HPP
#define TF_MODEL_IO_INTERFACES_HPP

#include <cstdlib>

#include <memory>

#include <tmdl/model_exception.hpp>
#include <tmdl/value.hpp>


namespace tmdl
{

class BlockExecutionInterface;

class BlockInterface
{
public:
    virtual size_t get_num_inputs() const = 0;

    virtual bool is_delayed_input(const size_t port) const
    {
        if (port < get_num_inputs())
        {
            return false;
        }
        else
        {
            throw ModelException("port number exceeds input port count");
        }
    }

    virtual size_t get_num_outputs() const = 0;

    virtual std::shared_ptr<BlockExecutionInterface> get_execution_interface() const = 0;
};

class BlockExecutionInterface
{
public:
    BlockExecutionInterface(const BlockInterface* parent) : parent(parent)
    {
        // Empty Constructor
    }

    virtual size_t get_num_inputs() const
    {
        return parent->get_num_inputs();
    }

    virtual size_t get_num_outputs() const
    {
        return parent->get_num_outputs();
    }

    virtual void step()
    {
        // Empty Step
    }

    virtual void reset()
    {
        // Empty Reset
    }

    virtual void set_input_value(
        const size_t port,
        const std::shared_ptr<const Value> value) = 0;

    virtual std::shared_ptr<const Value> get_output_value(const size_t port) const = 0;

protected:
    const BlockInterface* parent;
};


}

#endif // TF_MODEL_IO_INTERFACES_HPP
