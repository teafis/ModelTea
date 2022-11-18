// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_HPP
#define TF_MODEL_STDLIB_HPP

#include <tmdl/library.hpp>

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

    std::shared_ptr<LibraryBlock> create_block_from_name(const std::string& name) const override;

protected:
    std::unordered_map<std::string, std::shared_ptr<LibraryBlock> (*)()> block_map;
};

}

}

#endif // TF_MODEL_STDLIB_HPP
