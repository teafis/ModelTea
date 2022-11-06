// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_IO_INTERFACES_HPP
#define TF_MODEL_IO_INTERFACES_HPP

#include <cstdlib>

#include <memory>

#include <tmdl/value.hpp>


namespace tmdl
{

class BlockInterface
{
public:
    virtual size_t get_num_inputs() const = 0;

    virtual size_t get_num_outputs() const = 0;

    virtual void set_input_value(const size_t port, std::unique_ptr<Value> value) = 0;

    virtual std::unique_ptr<Value> get_output_value(const size_t port) const = 0;

    virtual void step() = 0;

    virtual void reset() = 0;
};

}

#endif // TF_MODEL_IO_INTERFACES_HPP
