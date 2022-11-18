// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_IO_INTERFACES_HPP
#define TF_MODEL_IO_INTERFACES_HPP

#include <cstdlib>

#include <memory>
#include <string>
#include <vector>

#include "model_exception.hpp"
#include "sim_state.hpp"
#include "value.hpp"


namespace tmdl
{

class BlockExecutionInterface;

struct BlockError
{
    size_t id;
    std::string message;
};

class BlockInterface
{
public:
    size_t get_id() const;

    void set_id(const size_t id);

    virtual bool update_block() = 0; // return true if the block updated anything that affects other blocks

    virtual std::unique_ptr<const BlockError> has_error() const = 0; // Return null if no error

    virtual size_t get_num_inputs() const = 0;

    virtual bool is_delayed_input(const size_t port) const;

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
    virtual void init();

    virtual void step(const SimState&);

    virtual void reset();

    virtual void close();
};


}

#endif // TF_MODEL_IO_INTERFACES_HPP
