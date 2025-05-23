// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNSTDLIB_HPP
#define MTEA_DYNSTDLIB_HPP

#include "library.hpp"

#include <functional>
#include <string>
#include <unordered_map>

namespace mtea {

namespace blocks {

class StandardLibrary : public LibraryBase {
public:
    StandardLibrary();

    bool has_block(std::string_view name) const override;

    const std::string get_library_name() const override;

    std::vector<std::string> get_block_names() const override;

    std::unique_ptr<BlockInterface> create_block(std::string_view name) const override;

private:
    inline static std::string library_name = "stdlib";
    std::unordered_map<std::string, std::function<std::unique_ptr<BlockInterface>()>> block_map;
};

}

}

#endif // MTEA_DYNSTDLIB_HPP
