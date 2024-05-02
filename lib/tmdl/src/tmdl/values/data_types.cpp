// SPDX-License-Identifier: GPL-3.0-only

#include "data_types.hpp"

#include <unordered_map>
#include <vector>

class DataTypeMap {
protected:
    DataTypeMap() {
        const std::vector<std::pair<tmdl::DataType, std::string>> type_vals{
            {tmdl::DataType::BOOL, "bool"},
            {tmdl::DataType::F64, "f64"},
            {tmdl::DataType::F32, "f32"},
            {tmdl::DataType::I8, "i8"},
            {tmdl::DataType::U8, "u8"},
            {tmdl::DataType::I16, "i16"},
            {tmdl::DataType::U16, "u16"},
            {tmdl::DataType::I32, "i32"},
            {tmdl::DataType::U32, "u32"},
            {tmdl::DataType::NONE, "none"},
        };

        for (const auto& [t, n] : type_vals) {
            name_to_type.insert({n, t});
            type_to_name.insert({t, n});
        }
    }

public:
    static const DataTypeMap& get_instance() {
        static DataTypeMap inst;
        return inst;
    }

    tmdl::DataType get_type(const std::string& name) const {
        const auto it = name_to_type.find(name);
        if (it != name_to_type.end()) {
            return it->second;
        } else {
            return tmdl::DataType::NONE;
        }
    }

    const std::string& get_name(const tmdl::DataType dt) const {
        const auto it = type_to_name.find(dt);
        if (it != type_to_name.end()) {
            return it->second;
        } else {
            const static std::string unknown_str = "unknown";
            return unknown_str;
        }
    }

protected:
    std::unordered_map<std::string, tmdl::DataType> name_to_type;
    std::unordered_map<tmdl::DataType, std::string> type_to_name;
};

std::string tmdl::data_type_to_string(const DataType dtype) { return DataTypeMap::get_instance().get_name(dtype); }

tmdl::DataType tmdl::data_type_from_string(const std::string& s) { return DataTypeMap::get_instance().get_type(s); }
