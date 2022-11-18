// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_LIBRARY_HPP
#define TF_MODEL_LIBRARY_HPP

#include <string>
#include <vector>

#include "block_interface.hpp"
#include "parameter.hpp"

namespace tmdl
{

class LibraryBlock : public BlockInterface
{
public:
    virtual std::string get_name() const = 0;

    virtual std::string get_description() const = 0;

    virtual std::vector<Parameter*> get_parameters() const;
};

class LibraryBase
{
public:
    virtual std::string get_library_name() const = 0;

    virtual std::vector<std::string> get_block_names() const = 0;

    virtual std::shared_ptr<LibraryBlock> create_block_from_name(const std::string& name) const = 0;
};

}

#endif // TF_MODEL_LIBRARY_HPP
