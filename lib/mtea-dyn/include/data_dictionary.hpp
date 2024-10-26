// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNDATA_DICTIONARY_HPP
#define MTEA_DYNDATA_DICTIONARY_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "codegen.hpp"
#include "value.hpp"
#include "identifier.hpp"

#include "mtea_types.hpp"

#include <nlohmann/json.hpp>

namespace mtea {

class DataDictionary {
public:
    DataDictionary() = default;

    void add_value(const Identifier& i, std::unique_ptr<ModelValue>&& val);

    ModelValue* get_value(const Identifier& i) const;

    std::unique_ptr<mtea::Argument> get_arg_ptr();

    std::vector<std::pair<Identifier, ModelValue*>> get_values() const;

    std::vector<std::string> write_code(codegen::CodeSection section) const;

    std::optional<std::string> name() const;

    void save() const;

    void save(const std::filesystem::path& path);

    static DataDictionary load(const std::filesystem::path& path);

private:
    std::unordered_map<Identifier, std::unique_ptr<ModelValue>, Identifier::Hasher> vals;
    std::optional<std::filesystem::path> save_path;
};

void to_json(nlohmann::json& j, const DataDictionary& d);
void from_json(const nlohmann::json& j, DataDictionary& d);

}

#endif // MTEA_DYNDATA_DICTIONARY
