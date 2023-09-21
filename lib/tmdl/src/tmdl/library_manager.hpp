// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_LIBRARY_MANAGER_HPP
#define TF_MODEL_LIBRARY_MANAGER_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "library.hpp"

#include "libraries/model_library.hpp"

namespace tmdl
{

class LibraryManager
{
public:
    static LibraryManager& get_instance(); // TODO - Remove Singleton

protected:
    LibraryManager();

public:
    void register_library(const std::string& name, std::shared_ptr<LibraryBase> library);

    void deregister_library(const std::string& name);

    std::shared_ptr<const LibraryBase> get_library(const std::string& name) const;

    std::vector<std::string> get_library_names() const;

    std::shared_ptr<BlockInterface> create_block(const std::string& name) const;

    std::shared_ptr<BlockInterface> try_create_block(const std::string& name) const;

    bool has_block(const std::string& name) const;

    std::shared_ptr<ModelLibrary> default_model_library() const;

private:
    std::unordered_map<std::string, std::shared_ptr<LibraryBase>> libraries;
    std::shared_ptr<ModelLibrary> model_library;
};

}

#endif // TF_MODEL_LIBRARY_MANAGER_HPP
