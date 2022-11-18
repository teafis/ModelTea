// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_HPP
#define TF_MODEL_STDLIB_HPP

#include <tmdl/library.hpp>


namespace tmdl
{

namespace stdlib
{

class StandardLibrary : public LibraryBase
{
public:
    std::string get_library_name() const override;

    std::vector<std::string> get_block_names() const override;

    std::shared_ptr<Block> create_block_from_name(const std::string& name) const override;
};

}

}

#endif // TF_MODEL_STDLIB_HPP
