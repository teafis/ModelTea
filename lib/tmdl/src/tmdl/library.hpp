// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_LIBRARY_HPP
#define TF_MODEL_LIBRARY_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "block_interface.hpp"

namespace tmdl
{

class LibraryBase
{
public:
    virtual ~LibraryBase();

    virtual std::string get_library_name() const = 0;

    virtual std::vector<std::string> get_block_names() const = 0;

    virtual bool has_block(const std::string name) const = 0;

    virtual std::shared_ptr<BlockInterface> create_block(const std::string& name) const = 0;
};

}

#endif // TF_MODEL_LIBRARY_HPP
