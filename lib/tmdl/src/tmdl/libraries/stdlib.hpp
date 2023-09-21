// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_HPP
#define TF_MODEL_STDLIB_HPP

#include "../library.hpp"

#include <string>
#include <unordered_map>
#include <functional>

namespace tmdl
{

namespace blocks
{

class StandardLibrary : public LibraryBase
{
public:
    StandardLibrary();

    bool has_block(std::string_view name) const;

    const std::string& get_library_name() const override;

    std::vector<std::string> get_block_names() const override;

    std::shared_ptr<BlockInterface> create_block(std::string_view name) const override;

private:
    inline static std::string library_name = "stdlib";
    std::unordered_map<std::string, std::shared_ptr<BlockInterface> (*)()> block_map;
};

}

}

#endif // TF_MODEL_STDLIB_HPP
