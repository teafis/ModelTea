// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_HPP
#define TF_MODEL_STDLIB_HPP

#include "library.hpp"

#include <string>
#include <unordered_map>
#include <functional>

namespace tmdl
{

namespace stdlib
{

class StandardLibrary : public LibraryBase
{
public:
    StandardLibrary();

    bool has_block(const std::string& name) const;

    std::string get_library_name() const override;

    std::vector<std::string> get_block_names() const override;

    bool has_block(const std::string name) const;

    std::shared_ptr<BlockInterface> create_block(const std::string& name) const override;

protected:
    std::unordered_map<std::string, std::shared_ptr<BlockInterface> (*)()> block_map;
};

}

}

#endif // TF_MODEL_STDLIB_HPP
