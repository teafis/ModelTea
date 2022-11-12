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

    virtual bool update_block() = 0; // return true if the block updated anything that affects other blocks

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

    virtual const PortValue* get_output_port(const size_t port) const = 0;

    virtual std::shared_ptr<BlockExecutionInterface> get_execution_interface() const = 0;

protected:
    size_t _id = 0;
};

class BlockExecutionInterface
{
public:
    virtual void init()
    {
        // Empty Init
    }

    virtual void step()
    {
        // Empty Step
    }

    virtual void reset()
    {
        // Empty Reset
    }

    virtual void close()
    {
        // Empty Close
    }
};


}

#endif // TF_MODEL_IO_INTERFACES_HPP
