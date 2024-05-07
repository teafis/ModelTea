// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_LIBRARY_MANAGER_HPP
#define TF_MODEL_LIBRARY_MANAGER_HPP

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "library.hpp"

#include "libraries/model_library.hpp"

namespace tmdl {

class LibraryManager {
public:
    static LibraryManager& get_instance(); // TODO - Remove Singleton

protected:
    LibraryManager();

public:
    void register_library(std::string_view name, std::shared_ptr<LibraryBase> library);

    void deregister_library(std::string_view name);

    LibraryBase* get_library(std::string_view name) const;

    std::vector<std::string> get_library_names() const;

    std::unique_ptr<BlockInterface> create_block(std::string_view name) const;

    std::unique_ptr<BlockInterface> try_create_block(std::string_view name) const;

    bool has_block(std::string_view name) const;

    ModelLibrary* default_model_library() const;

private:
    std::unordered_map<std::string, std::shared_ptr<LibraryBase>> libraries;
    std::shared_ptr<ModelLibrary> model_library;
};

}

#endif // TF_MODEL_LIBRARY_MANAGER_HPP
