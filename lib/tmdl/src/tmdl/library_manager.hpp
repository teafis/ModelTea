// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_LIBRARY_MANAGER_HPP
#define TF_MODEL_LIBRARY_MANAGER_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "library.hpp"

namespace tmdl
{

class LibraryManager
{
public:
    static LibraryManager& get_instance();

protected:
    LibraryManager();

public:
    void register_library(const std::string& name, std::shared_ptr<LibraryBase> library);

    void deregister_library(const std::string& name);

    std::shared_ptr<const LibraryBase> get_library(const std::string& name) const;

    std::vector<std::string> get_library_names() const;

protected:
    std::unordered_map<std::string, std::shared_ptr<LibraryBase>> libraries;
};

}

#endif // TF_MODEL_LIBRARY_MANAGER_HPP
