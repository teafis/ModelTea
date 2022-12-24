// SPDX-License-Identifier: GPL-3.0-only

#include "library_manager.hpp"

#include "libraries/stdlib.hpp"

#include "model_exception.hpp"

#include <algorithm>


tmdl::LibraryManager& tmdl::LibraryManager::get_instance()
{
    static LibraryManager instance;
    return instance;
}

tmdl::LibraryManager::LibraryManager()
{
    model_library = std::make_shared<ModelLibrary>();

    register_library("stdlib", std::make_shared<tmdl::stdlib::StandardLibrary>());
    register_library("models", model_library);
}

void tmdl::LibraryManager::register_library(const std::string& name, std::shared_ptr<LibraryBase> library)
{
    const auto it = libraries.find(name);
    if (it != libraries.end())
    {
        throw ModelException("library name already exists in manager");
    }

    libraries.insert({name, library});
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
    for (const auto& kv : libraries)
    {
        names.push_back(kv.first);
    }
    std::sort(names.begin(), names.end());
    return names;
}

std::shared_ptr<tmdl::BlockInterface> tmdl::LibraryManager::make_block(const std::string& name) const
{
    for (const auto& kv : libraries)
    {
        if (kv.second->has_block(name))
        {
            return kv.second->create_block(name);
        }
    }

    throw ModelException("unable to create block with name");
}

std::shared_ptr<tmdl::ModelLibrary> tmdl::LibraryManager::default_model_library() const
{
    return model_library;
}
