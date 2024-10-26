// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNLIBRARY_HPP
#define MTEA_DYNLIBRARY_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "block_interface.hpp"

namespace mtea {

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

#endif // MTEA_DYNLIBRARY_HPP
