// SPDX-License-Identifier: GPL-3.0-only

#include "library.hpp"

std::unique_ptr<mtea::BlockInterface> mtea::LibraryBase::try_create_block(std::string_view name) const {
    if (has_block(name)) {
        return create_block(name);
    } else {
        return nullptr;
    }
}
