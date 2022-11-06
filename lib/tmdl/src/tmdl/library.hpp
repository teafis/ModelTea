// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_LIBRARY_HPP
#define TF_MODEL_LIBRARY_HPP

#include <string>
#include <vector>

#include <tmdl/block.hpp>

namespace tmdl
{

class LibraryBase
{
public:
    virtual std::vector<std::string> get_block_names() const = 0;

    virtual std::unique_ptr<Block> create_block_from_name(const std::string& name) const = 0;
};

}

#endif // TF_MODEL_LIBRARY_HPP
