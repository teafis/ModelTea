// SPDX-License-Identifier: GPL-3.0-only

#include "stdlib.hpp"

#include "../model_exception.hpp"

#include "../blocks/io_ports.hpp"
#include "../values/parameter.hpp"

#include <mtstd.hpp>
#include <mtstd_creation.hpp>

#include <ranges>

template <typename T> std::unique_ptr<tmdl::BlockInterface> make_block() { return std::make_unique<T>(); }

class StdlibBlock : public tmdl::BlockInterface {

public:
    StdlibBlock(const mt::stdlib::BlockInformation& info) : info{info}, friendly_name{} {
        const auto init_dt = info.get_default_data_type();

        if (info.constructor == mt::stdlib::BlockInformation::ConstructorOptions::SIZE) {
            param_size = std::make_shared<tmdl::ParameterValue>("size", "Block Size",
                                                                std::make_unique<tmdl::ModelValueBox<mt::stdlib::DataType::U32>>(2));
        } else if (info.constructor == mt::stdlib::BlockInformation::ConstructorOptions::VALUE) {
            param_dt = std::make_shared<tmdl::ParameterDataType>("dtype", "Data Type", init_dt);
            param_value = std::make_shared<tmdl::ParameterValue>("value", "Value", tmdl::ModelValue::make_default(init_dt));
        }

        update_block(init_dt);
    }

    StdlibBlock(const mt::stdlib::BlockInformation& info, const std::string& friendly_name) : StdlibBlock(info) {
        this->friendly_name = friendly_name;
    }

    std::string get_name() const override { return friendly_name.value_or(info.sub_name.empty() ? info.base_name : info.sub_name); }

    std::string get_description() const override { return fmt::format("STDLIB BLOCK - {}", get_name()); }

    std::vector<std::shared_ptr<tmdl::Parameter>> get_parameters() const override {
        std::vector<std::shared_ptr<tmdl::Parameter>> params;

        if (param_size) {
            params.push_back(param_size);
        }

        if (param_dt) {
            params.push_back(param_dt);
        }

        if (param_value) {
            params.push_back(param_value);
        }

        return params;
    }

    mt::stdlib::DataType selected_type() const {
        if (param_dt != nullptr) {
            return param_dt->get_type();
        } else if (block != nullptr) {
            return block->get_current_type();
        } else {
            return info.get_default_data_type();
        }
    }

    bool update_block() override { return update_block(selected_type()); }

    bool update_block(tmdl::DataType new_dtype) {
        // Create the argument type
        std::unique_ptr<const mt::stdlib::Argument> arg = nullptr;

        if (info.constructor == mt::stdlib::BlockInformation::ConstructorOptions::SIZE) {
            const uint32_t size_val = tmdl::ModelValue::get_inner_value<mt::stdlib::DataType::U32>(param_size->get_value());
            arg = std::make_unique<mt::stdlib::ArgumentBox<mt::stdlib::DataType::U32>>(size_val);
        } else if (info.constructor == mt::stdlib::BlockInformation::ConstructorOptions::VALUE) {
            param_value->convert_type(param_dt->get_type());
            arg = param_value->get_value()->to_argument();
        } else if (info.constructor == mt::stdlib::BlockInformation::ConstructorOptions::TIMESTEP) {
            arg = tmdl::ModelValue::make_default(new_dtype)->to_argument();
        }

        // Create the new block
        std::unique_ptr<mt::stdlib::block_interface> new_block = nullptr;
        try {
            new_block = mt::stdlib::create_block(info, new_dtype, arg.get());
        } catch (const mt::stdlib::block_error& err) {
            return false;
        }

        const auto ext_dt = (block) ? block->get_current_type() : mt::stdlib::DataType::NONE;
        const auto new_dt = new_block->get_current_type();

        // Return whether any interface parameters have changed
        const bool interface_changed = block == nullptr || block->get_input_num() != new_block->get_input_num() ||
                                       block->get_output_num() != block->get_output_num() ||
                                       block->get_current_type() != new_block->get_current_type() ||
                                       input_types.size() != new_block->get_input_num();

        // Create the dtype parameter if needed
        if (new_block->get_input_num() == 0 && !param_dt) {
            param_dt = std::make_shared<tmdl::ParameterDataType>("dtype", "Data Type", new_block->get_current_type());
        }

        // Resize the input types as needed
        input_types.resize(new_block->get_input_num(), mt::stdlib::DataType::NONE);

        // Update the block and return
        block = std::move(new_block);
        return interface_changed;
    }

    static const char* get_meta_type_name(const mt::stdlib::DataType dt) {
        const auto mt = mt::stdlib::get_meta_type(dt);
        if (mt == nullptr) {
            return "none";
        } else {
            return mt->get_name();
        }
    }

    std::unique_ptr<const tmdl::BlockError> has_error() const override {
        if (param_dt != nullptr && param_dt->get_type() != block->get_current_type()) {
            return make_error(fmt::format("unsupported type provided - {} != {}", get_meta_type_name(param_dt->get_type()),
                                          get_meta_type_name(block->get_current_type())));
        }

        if (input_types.size() != block->get_input_num()) {
            return make_error("input size doesn't match inner block input size");
        }

        for (size_t i = 0; i < get_num_inputs(); ++i) {
            const auto current = input_types[i];
            const auto expected = block->get_input_type(i);

            if (current != expected) {
                return make_error(fmt::format("port {} with type {} doens't match expected type {}", i, get_meta_type_name(current),
                                              get_meta_type_name(expected)));
            }
        }

        return nullptr;
    }

    size_t get_num_inputs() const override { return input_types.size(); }

    size_t get_num_outputs() const override { return block->get_output_num(); }

    bool outputs_are_delayed() const override { return block->outputs_are_delayed(); }

    void set_input_type(const size_t port, const tmdl::DataType type) override {
        if (port < input_types.size()) {
            if (input_types[port] != type) {
                if (param_dt == nullptr && block->get_input_type_settable(port) && info.type_supported(type) &&
                    block->get_current_type() != type) {
                    update_block(type);
                }

                input_types[port] = type;
            }
        } else {
            throw tmdl::ModelException("input type exceeds bounds");
        }
    }

    tmdl::DataType get_output_type(const size_t port) const override { return block->get_output_type(port); }

    std::unique_ptr<tmdl::CompiledBlockInterface> get_compiled(const ModelInfo& s) const override {
        throw tmdl::ModelException("not implemented");
    }

private:
    mt::stdlib::BlockInformation info;
    std::unique_ptr<mt::stdlib::block_interface> block{nullptr};

    std::shared_ptr<tmdl::ParameterValue> param_size{};
    std::shared_ptr<tmdl::ParameterValue> param_value{};
    std::shared_ptr<tmdl::ParameterDataType> param_dt{};

    std::vector<std::shared_ptr<tmdl::Parameter>> parameters{};
    std::optional<std::string> friendly_name;

    std::vector<mt::stdlib::DataType> input_types;
};

tmdl::blocks::StandardLibrary::StandardLibrary() {
    block_map = {{"input", &make_block<InputPort>}, {"output", &make_block<OutputPort>}};

    for (const auto& blk : mt::stdlib::get_available_blocks()) {
        block_map[blk.sub_name.empty() ? blk.base_name : blk.sub_name] = [blk]() { return std::make_unique<StdlibBlock>(blk); };
    }
}

bool tmdl::blocks::StandardLibrary::has_block(const std::string_view name) const { return block_map.contains(std::string(name)); }

const std::string tmdl::blocks::StandardLibrary::get_library_name() const { return library_name; }

std::vector<std::string> tmdl::blocks::StandardLibrary::get_block_names() const {
    std::vector<std::string> keys;
    for (const auto& k : block_map | std::views::keys) {
        keys.push_back(k);
    }
    std::ranges::sort(keys);
    return keys;
}

std::unique_ptr<tmdl::BlockInterface> tmdl::blocks::StandardLibrary::create_block(const std::string_view name) const {
    if (auto it = block_map.find(std::string(name)); it != block_map.end()) {
        return it->second();
    } else {
        throw ModelException("unknown block type provided to library");
    }
}
