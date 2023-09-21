// SPDX-License-Identifier: GPL-3.0-only

#include "library_manager.hpp"

#include "libraries/stdlib.hpp"

#include "model_exception.hpp"

#include <algorithm>

#include <fmt/format.h>


tmdl::LibraryManager& tmdl::LibraryManager::get_instance()
{
    static LibraryManager instance;
    return instance;
}

tmdl::LibraryManager::LibraryManager()
{
    model_library = std::make_shared<ModelLibrary>();

    register_library("stdlib", std::make_shared<tmdl::blocks::StandardLibrary>());
    register_library("models", model_library);
}

void tmdl::LibraryManager::register_library(const std::string& name, std::shared_ptr<LibraryBase> library)
{
    if (const auto it = libraries.find(name); it != libraries.end())
    {
        throw ModelException("library name already exists in manager");
    }

    libraries.try_emplace(name, library);
}

void tmdl::LibraryManager::deregister_library(const std::string& name)
{
    const auto it = libraries.find(name);
    if (it == libraries.end())
    {
        throw ModelException("library name does not exist in manager");
    }

    libraries.erase(it);
}

std::shared_ptr<const tmdl::LibraryBase> tmdl::LibraryManager::get_library(const std::string& name) const
{
    const auto it = libraries.find(name);
    if (it == libraries.end())
    {
        throw ModelException("library name does not exists in manager");
    }
    else
    {
        return it->second;
    }
}

std::vector<std::string> tmdl::LibraryManager::get_library_names() const
{
    std::vector<std::string> names;
    for (const auto& k : libraries | std::views::keys)
    {
        names.push_back(k);
    }
    std::ranges::sort(names);
    return names;
}

std::shared_ptr<tmdl::BlockInterface> tmdl::LibraryManager::create_block(const std::string& name) const
{
    if (auto blk = try_create_block(name))
    {
        return blk;
    }
    else
    {
        throw ModelException(fmt::format("no block with name `{}` found", name));
    }
}

std::shared_ptr<tmdl::BlockInterface> tmdl::LibraryManager::try_create_block(const std::string& name) const
{
    for (const auto& lib : libraries | std::views::values)
    {
        if (lib->has_block(name))
        {
            return lib->create_block(name);
        }
    }

    return nullptr;
}

bool tmdl::LibraryManager::has_block(const std::string& name) const
{
    return try_create_block(name) != nullptr;
}

std::shared_ptr<tmdl::ModelLibrary> tmdl::LibraryManager::default_model_library() const
{
    return model_library;
}
