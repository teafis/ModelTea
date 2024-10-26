// SPDX-License-Identifier: GPL-3.0-only

#include "data_dictionary.hpp"

#include <fstream>
#include <iomanip>

#include <fmt/format.h>

void mtea::DataDictionary::add_value(const Identifier& i, std::unique_ptr<ModelValue>&& val) {
    vals.emplace(std::make_pair(i, std::move(val)));
}

mtea::ModelValue* mtea::DataDictionary::get_value(const Identifier& i) const {
    if (const auto it = vals.find(i); it != vals.end()) {
        return it->second.get();
    } else {
        return nullptr;
    }
}

std::vector<std::pair<mtea::Identifier, mtea::ModelValue*>> mtea::DataDictionary::get_values() const {
    std::vector<std::pair<Identifier, ModelValue*>> rvals{};
    for (const auto& [k, v] : vals) {
        rvals.emplace_back(std::make_pair(k, v.get()));
    }
    return rvals;
}

std::vector<std::string> mtea::DataDictionary::write_code(const codegen::CodeSection section) const {
    std::vector<std::string> output{};

    for (const auto& [k, v] : vals) {
        const auto dt = codegen::get_datatype_name(v->data_type());
        const auto i = k.get();

        if (section == codegen::CodeSection::DECLARATION) {
            output.push_back(fmt::format("extern {} {};", dt, i));
        } else if (section == codegen::CodeSection::DEFINITION) {
            output.push_back(fmt::format("{} {} = {};", dt, i, dt));
        } else {
            throw codegen::CodegenError(fmt::format("unknown code section provided"));
        }
    }

    return output;
}

std::optional<std::string> mtea::DataDictionary::name() const {
    if (save_path.has_value()) {
        return save_path->stem();
    } else {
        return {};
    }
}

void mtea::DataDictionary::save() const {
    if (save_path.has_value()) {
        nlohmann::json j;
        j["dict"] = *this;

        std::ofstream oss(*save_path);
        oss << std::setw(4) << j;
    } else {
        throw ModelException(fmt::format("no save path provided for dictionary"));
    }
}

void mtea::DataDictionary::save(const std::filesystem::path& path) {
    save_path = path;
    save();
}

mtea::DataDictionary mtea::DataDictionary::load(const std::filesystem::path& path) {
    DataDictionary d;
    nlohmann::json j;

    std::ifstream iss(path);

    try {
        iss >> j;
    } catch (const nlohmann::json::exception& err) {
        throw ModelException(fmt::format("unable to load dictionary '{}' - {}", path.string(), err.what()));
    }

    from_json(j["dict"], d);
    d.save_path = path;

    return d;
}

struct ValueStorage {
    ValueStorage() = default;
    ValueStorage(const mtea::ModelValue* ptr) {
        value = ptr->to_string();
        dtype = ptr->data_type();
    }

    std::unique_ptr<mtea::ModelValue> to_value() const { return mtea::ModelValue::from_string(value, dtype); }

    std::string value{""};
    mtea::DataType dtype{mtea::DataType::NONE};
};

void to_json(nlohmann::json& j, const ValueStorage& s) {
    j["value"] = s.value;
    j["dtype"] = s.dtype;
}

void from_json(const nlohmann::json& j, ValueStorage& s) {
    j.at("value").get_to(s.value);
    j.at("dtype").get_to(s.dtype);
}

void mtea::to_json(nlohmann::json& j, const DataDictionary& d) {
    std::unordered_map<std::string, ValueStorage> vals;

    for (const auto [k, v] : d.get_values()) {
        vals.emplace(std::make_pair(k.get(), ValueStorage(v)));
    }

    j["parameters"] = vals;
}

void mtea::from_json(const nlohmann::json& j, DataDictionary& d) {
    std::unordered_map<std::string, ValueStorage> vals;
    j.at("parameters").get_to(vals);

    for (const auto [k, v] : vals) {
        d.add_value(Identifier(k), mtea::ModelValue::from_string(v.value, v.dtype));
    }
}
