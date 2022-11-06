// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCK_HPP
#define TF_MODEL_BLOCK_HPP

#include <cstdlib>

#include <string>
#include <vector>

#include <tmdl/block_interface.hpp>

#include <tmdl/parameter.hpp>
#include <tmdl/value.hpp>


namespace tmdl
{

class Block : public BlockInterface
{
public:
    std::string get_name() const;

    std::string get_description() const;

    std::vector<Parameter*> get_parameters() const;

    size_t get_num_inputs() const;

    size_t get_num_outputs() const;

    void set_input_value(const size_t port, std::unique_ptr<Value> value);

    std::unique_ptr<Value> get_output_value(const size_t port) const;

    void step();

    void reset();
};

}

#endif // TF_MODEL_BLOCK_HPP
