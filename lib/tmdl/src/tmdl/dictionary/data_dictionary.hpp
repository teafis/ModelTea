// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_DATA_DICTIONARY_HPP
#define TF_MODEL_DATA_DICTIONARY_HPP

#include <filesystem>
#include <optional>
#include <unordered_map>

#include "data_parameter.hpp"

namespace tmdl {

class DataDictionary {
public:
    DataDictionary() = default;

private:
    std::unordered_map<std::string, std::unique_ptr<DataParameter>> vals;
    std::optional<std::filesystem::path> save_path;
};

}

#endif // TF_MODEL_DATA_DICTIONARY
