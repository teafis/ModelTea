// SPDX-License-Identifier: GPL-3.0-only

module;

#include <string>
#include <unordered_map>
#include <vector>

export module tmdl:library;

import :block_interface;

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

std::unique_ptr<tmdl::BlockInterface> tmdl::LibraryBase::try_create_block(std::string_view name) const {
    if (has_block(name)) {
        return create_block(name);
    } else {
        return nullptr;
    }
}
