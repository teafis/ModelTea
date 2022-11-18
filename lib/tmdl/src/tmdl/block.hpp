// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCK_HPP
#define TF_MODEL_BLOCK_HPP

#include <cstdlib>

#include <string>
#include <vector>

#include "block_interface.hpp"

#include "parameter.hpp"
#include "value.hpp"


namespace tmdl
{

class Block : public BlockInterface
{
public:
    virtual std::string get_name() const = 0;

    virtual std::string get_description() const = 0;

    virtual std::vector<Parameter*> get_parameters() const;
};

}

#endif // TF_MODEL_BLOCK_HPP
