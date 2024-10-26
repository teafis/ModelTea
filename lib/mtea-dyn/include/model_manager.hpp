// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNMANAGER_HPP
#define MTEA_DYNMANAGER_HPP

#include <string>
#include <string_view>
#include <vector>

#include "data_dictionary.hpp"
#include "library.hpp"
#include "library_model.hpp"

namespace mtea {

class ModelManager {
public:
    static ModelManager& get_instance(); // TODO - Remove Singleton

protected:
    ModelManager();

    ModelManager(const ModelManager&) = delete;
    ModelManager(ModelManager&&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;
    ModelManager& operator=(ModelManager&&) = delete;

public:
    LibraryBase* register_library(std::string_view name, std::unique_ptr<LibraryBase>&& library);

    std::unique_ptr<LibraryBase> deregister_library(std::string_view name);

    LibraryBase* get_library(std::string_view name) const;

    std::vector<std::string> get_library_names() const;

    std::unique_ptr<BlockInterface> create_block(std::string_view name) const;

    std::unique_ptr<BlockInterface> try_create_block(std::string_view name) const;

    bool has_block(std::string_view name) const;

    ModelLibrary* default_model_library() const;

    void dict_register(std::string_view name, std::unique_ptr<DataDictionary>&& data);

    void dict_close(std::string_view name);

    DataDictionary* get_data(std::string_view name) const;

    ModelValue* get_dictionary_value(std::string_view name) const;

private:
    template <typename T> T* register_library_type(std::string_view name, std::unique_ptr<T>&& library) {
        const auto sname = std::string(name);
        auto ptr = library.get();

        if (const auto it = libraries.find(sname); it != libraries.end()) {
            throw ModelException("library name already exists in manager");
        }

        libraries.emplace(std::make_pair(sname, std::move(library)));
        return ptr;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<LibraryBase>> libraries;
    std::unordered_map<std::string, std::unique_ptr<DataDictionary>> dictionaries;
    ModelLibrary* model_library;
};

}

#endif // MTEA_DYNMANAGER_HPP
