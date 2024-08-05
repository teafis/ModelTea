// SPDX-License-Identifier: GPL-3.0-only

module;

#include <filesystem>
#include <optional>
#include <unordered_map>

export module tmdl.dictionary:data_dictionary;

import :data_parameter;

namespace tmdl {

export class DataDictionary {
public:
    DataDictionary() = default;

private:
    std::unordered_map<std::string, std::unique_ptr<DataParameter>> vals;
    std::optional<std::filesystem::path> save_path;
};

}
