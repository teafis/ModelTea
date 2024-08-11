// SPDX-License-Identifier: GPL-3.0-only

#include "library_manager.hpp"

#include <algorithm>

#include <fmt/format.h>

#include "library_stdlib.hpp"
#include "model_exception.hpp"

tmdl::LibraryManager& tmdl::LibraryManager::get_instance() {
    static LibraryManager instance;
    return instance;
}

tmdl::LibraryManager::LibraryManager() {
    model_library = std::make_shared<ModelLibrary>();

    register_library("stdlib", std::make_shared<tmdl::blocks::StandardLibrary>());
    register_library("models", model_library);
}

void tmdl::LibraryManager::register_library(std::string_view name, std::shared_ptr<LibraryBase> library) {
    const auto sname = std::string(name);

    if (const auto it = libraries.find(sname); it != libraries.end()) {
        throw ModelException("library name already exists in manager");
    }

    libraries.try_emplace(sname, library);
}

void tmdl::LibraryManager::deregister_library(std::string_view name) {
    const auto it = libraries.find(std::string(name));
    if (it == libraries.end()) {
        throw ModelException("library name does not exist in manager");
    }

    libraries.erase(it);
}

tmdl::LibraryBase* tmdl::LibraryManager::get_library(std::string_view name) const {
    const auto it = libraries.find(std::string(name));
    if (it == libraries.end()) {
        throw ModelException("library name does not exists in manager");
    } else {
        return it->second.get();
    }
}

std::vector<std::string> tmdl::LibraryManager::get_library_names() const {
    std::vector<std::string> names;
    for (const auto& k : libraries | std::views::keys) {
        names.push_back(k);
    }
    std::ranges::sort(names);
    return names;
}

std::unique_ptr<tmdl::BlockInterface> tmdl::LibraryManager::create_block(std::string_view name) const {
    if (auto blk = try_create_block(name)) {
        return blk;
    } else {
        throw ModelException(fmt::format("no block with name `{}` found", name));
    }
}
std::unique_ptr<tmdl::BlockInterface> tmdl::LibraryManager::try_create_block(std::string_view name) const {
    const std::string SCOPE_SYMBOL = "::";

    const auto it = name.find(SCOPE_SYMBOL);
    if (it != std::string::npos) {
        const auto lib_name = name.substr(0, it);
        const auto block_name = name.substr(it + SCOPE_SYMBOL.size());

        const auto lib = get_library(lib_name);
        if (lib) {
            return lib->create_block(block_name);
        }
    } else {
        for (const auto& lib : libraries | std::views::values) {
            if (lib->has_block(name)) {
                return lib->create_block(name);
            }
        }
    }

    return nullptr;
}

bool tmdl::LibraryManager::has_block(std::string_view name) const { return try_create_block(name) != nullptr; }

tmdl::ModelLibrary* tmdl::LibraryManager::default_model_library() const { return model_library.get(); }
