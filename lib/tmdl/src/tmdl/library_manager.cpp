// SPDX-License-Identifier: GPL-3.0-only

#include "library_manager.hpp"

#include "stdlib.hpp"

#include "model_exception.hpp"

#include <algorithm>


tmdl::LibraryManager& tmdl::LibraryManager::get_instance()
{
    static LibraryManager instance;
    return instance;
}

tmdl::LibraryManager::LibraryManager()
{
    register_library("stdlib", std::make_shared<tmdl::stdlib::StandardLibrary>());
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
