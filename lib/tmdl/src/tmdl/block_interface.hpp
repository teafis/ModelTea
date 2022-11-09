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
    size_t get_id() const
    {
        return _id;
    }

    void set_id(const size_t id)
    {
        _id = id;
    }

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

    virtual void set_input_port(
        const size_t port,
        const PortValue* value) = 0;

    virtual void set_output_port(
        const size_t port,
        PortValue* value) = 0;

    virtual BlockExecutionInterface* get_executor() const = 0;

protected:
    size_t _id = 0;
};

class BlockExecutionInterface
{
public:
    BlockExecutionInterface(const BlockInterface* parent) : parent(parent)
    {
        // Empty Constructor
    }

    virtual void step()
    {
        // Empty Step
    }

    virtual void reset()
    {
        // Empty Reset
    }

    virtual const void* get_output_value() const = 0;

protected:
    const BlockInterface* parent;
};


}

#endif // TF_MODEL_IO_INTERFACES_HPP
