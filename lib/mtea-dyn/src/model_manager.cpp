// SPDX-License-Identifier: GPL-3.0-only

#include "model_manager.hpp"

#include <algorithm>

#include <fmt/format.h>

#include "library_stdlib.hpp"
#include "model_exception.hpp"

mtea::ModelManager& mtea::ModelManager::get_instance() {
    static ModelManager instance;
    return instance;
}

mtea::ModelManager::ModelManager() {
    register_library("stdlib", std::make_unique<mtea::blocks::StandardLibrary>());
    model_library = register_library_type("models", std::make_unique<ModelLibrary>());
}

mtea::LibraryBase* mtea::ModelManager::register_library(std::string_view name, std::unique_ptr<LibraryBase>&& library) {
    return register_library_type(name, std::move(library));
}

std::unique_ptr<mtea::LibraryBase> mtea::ModelManager::deregister_library(std::string_view name) {
    const auto it = libraries.find(std::string(name));
    if (it == libraries.end()) {
        throw ModelException("library name does not exist in manager");
    }

    auto ptr = std::move(it->second);
    libraries.erase(it);
    return ptr;
}

mtea::LibraryBase* mtea::ModelManager::get_library(std::string_view name) const {
    const auto it = libraries.find(std::string(name));
    if (it == libraries.end()) {
        throw ModelException("library name does not exists in manager");
    } else {
        return it->second.get();
    }
}

std::vector<std::string> mtea::ModelManager::get_library_names() const {
    std::vector<std::string> names;
    for (const auto& k : libraries | std::views::keys) {
        names.push_back(k);
    }
    std::ranges::sort(names);
    return names;
}

std::unique_ptr<mtea::BlockInterface> mtea::ModelManager::create_block(std::string_view name) const {
    if (auto blk = try_create_block(name)) {
        return blk;
    } else {
        throw ModelException(fmt::format("no block with name `{}` found", name));
    }
}
std::unique_ptr<mtea::BlockInterface> mtea::ModelManager::try_create_block(std::string_view name) const {
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

bool mtea::ModelManager::has_block(std::string_view name) const { return try_create_block(name) != nullptr; }

mtea::ModelLibrary* mtea::ModelManager::default_model_library() const { return model_library; }

void mtea::ModelManager::dict_register(std::string_view name, std::unique_ptr<DataDictionary>&& data) {
    if (auto it = dictionaries.find(std::string(name)); it != dictionaries.end()) {
        throw ModelException("data name already exists in manager");
    } else {
        dictionaries.emplace(std::make_pair(name, std::move(data)));
    }
}

void mtea::ModelManager::dict_close(std::string_view name) {
    if (auto it = dictionaries.find(std::string(name)); it != dictionaries.end()) {
        dictionaries.erase(it);
    } else {
        throw ModelException("data name does not exists in manager");
    }
}

mtea::DataDictionary* mtea::ModelManager::get_data(std::string_view name) const {
    if (const auto it = dictionaries.find(std::string(name)); it != dictionaries.end()) {
        return it->second.get();
    } else {
        throw ModelException("data name does not exists in manager");
    }
}
