// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCK_HPP
#define TF_MODEL_BLOCK_HPP

#include <cstdlib>

#include <string>
#include <vector>

#include <tmdl/parameter.hpp>
#include <tmdl/value.hpp>


namespace tmdl
{

class Block
{
public:
    size_t get_id() const;

    std::string get_name() const;

    std::string get_description() const;

    std::vector<Parameter*> get_parameters() const;

    void reset_compute_state();

    void reset();

    void set_input_value(const size_t port_num, const Value* value) const;

    const Value* get_output_value(const size_t port_num) const;
};

}

#endif // TF_MODEL_BLOCK_HPP
