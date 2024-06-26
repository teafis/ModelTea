// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_LIBRARY_HPP
#define TF_MODEL_LIBRARY_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "block_interface.hpp"

namespace tmdl {

class LibraryBase {
public:
    virtual ~LibraryBase() = default;

    virtual const std::string get_library_name() const = 0;

    virtual std::vector<std::string> get_block_names() const = 0;

    virtual bool has_block(std::string_view name) const = 0;

    virtual std::unique_ptr<BlockInterface> create_block(std::string_view name) const = 0;

    virtual std::unique_ptr<BlockInterface> try_create_block(std::string_view name) const;
};

}

#endif // TF_MODEL_LIBRARY_HPP
