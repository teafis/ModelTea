// SPDX-License-Identifier: GPL-3.0-only

#include "stdlib.hpp"

#include "../model_exception.hpp"

#include "../blocks/io_ports.hpp"
#include "../values/parameter.hpp"

#include "mtstdlib_creation.hpp"

#include <ranges>

struct StdlibBlockConstructor {
    StdlibBlockConstructor(const mt::stdlib::BlockInformation& info) : info{info} {
        const auto init_dt = info.get_default_data_type();

        if (info.constructor == mt::stdlib::BlockInformation::ConstructorOptions::SIZE) {
            param_size = std::make_shared<tmdl::ParameterValue>("size", "Block Size",
                                                                std::make_unique<tmdl::ModelValueBox<mt::stdlib::DataType::U32>>(2));
        } else if (info.constructor == mt::stdlib::BlockInformation::ConstructorOptions::VALUE) {
            param_dt = std::make_shared<tmdl::ParameterDataType>("dtype", "Data Type", init_dt);
            param_value = std::make_shared<tmdl::ParameterValue>("value", "Value", tmdl::ModelValue::make_default(init_dt));
        }

        // Create the dtype parameter if needed
        if (const auto blk = create_block(init_dt, 0.0)) {
            if (blk->get_input_num() == 0) {
                param_dt = std::make_shared<tmdl::ParameterDataType>("dtype", "Data Type", blk->get_current_type());
            }
        }
    }

    mt::stdlib::BlockInformation info;

    std::shared_ptr<tmdl::ParameterValue> param_size{};
    std::shared_ptr<tmdl::ParameterValue> param_value{};
    std::shared_ptr<tmdl::ParameterDataType> param_dt{};

    std::optional<mt::stdlib::DataType> selected_type() const {
        if (param_dt != nullptr) {
            return param_dt->get_type();
        } else {
            return {};
        }
    }

    std::unique_ptr<mt::stdlib::block_interface> create_block(mt::stdlib::DataType dtype, double dt) const {
        // Create the argument type
        std::unique_ptr<const mt::stdlib::Argument> arg = nullptr;

        if (info.constructor == mt::stdlib::BlockInformation::ConstructorOptions::SIZE) {
            const uint32_t size_val = tmdl::ModelValue::get_inner_value<mt::stdlib::DataType::U32>(param_size->get_value());
            arg = std::make_unique<mt::stdlib::ArgumentBox<mt::stdlib::DataType::U32>>(size_val);
        } else if (info.constructor == mt::stdlib::BlockInformation::ConstructorOptions::VALUE) {
            param_value->convert_type(param_dt->get_type());
            arg = param_value->get_value()->to_argument();
        } else if (info.constructor == mt::stdlib::BlockInformation::ConstructorOptions::TIMESTEP) {
            auto mv = tmdl::ModelValue::make_default(dtype);
            if (const auto ptr = dynamic_cast<tmdl::ModelValueBox<tmdl::DataType::F64>*>(mv.get())) {
                ptr->value = dt;
            } else if (const auto ptr = dynamic_cast<tmdl::ModelValueBox<tmdl::DataType::F32>*>(mv.get())) {
                ptr->value = static_cast<float>(dt);
            } else {
                throw tmdl::ModelException("unable to set timestep for type");
            }

            arg = mv->to_argument();
        }

        // Create the new block
        std::unique_ptr<mt::stdlib::block_interface> new_block = nullptr;
        try {
            return mt::stdlib::create_block(info, dtype, arg.get());
        } catch (const mt::stdlib::block_error& err) {
            throw tmdl::ModelException(err);
        }
    }
};

class StdlibBlockExecutor final : public tmdl::BlockExecutionInterface {
public:
    StdlibBlockExecutor(std::unique_ptr<mt::stdlib::block_interface>&& block_in,
                        std::vector<std::shared_ptr<const tmdl::ModelValue>>&& inputs_in,
                        std::vector<std::shared_ptr<tmdl::ModelValue>>&& outputs_in)
        : block{std::move(block_in)}, inputs(std::move(inputs_in)), outputs(std::move(outputs_in)) {
        if (!block) {
            throw tmdl::ModelException("block cannot be null");
        } else if (inputs.size() != block->get_input_num()) {
            throw tmdl::ModelException("block has incorrect number of inputs");
        } else if (outputs.size() != block->get_output_num()) {
            throw tmdl::ModelException("block has incorrect number of outputs");
        }
    }

protected:
    void update_inputs() override {
        for (size_t i = 0; i < inputs.size(); ++i) {
            block->set_input(i, inputs[i]->to_argument().get());
        }
    }

    void update_outputs() override {
        for (size_t i = 0; i < outputs.size(); ++i) {
            auto arg = outputs[i]->to_argument();
            block->get_output(i, arg.get());
            outputs[i]->copy_from(arg.get());
        }
    }

    void blk_reset() override { block->reset(); }

    void blk_step() override { block->step(); }

private:
    std::unique_ptr<mt::stdlib::block_interface> block;
    std::vector<std::shared_ptr<const tmdl::ModelValue>> inputs;
    std::vector<std::shared_ptr<tmdl::ModelValue>> outputs;
};

class StdlibBlockComponent final : public tmdl::codegen::CodeComponent {
public:
    StdlibBlockComponent(std::unique_ptr<mt::stdlib::block_interface>&& block_in) : block{std::move(block_in)} {
        if (!this->block) {
            throw tmdl::ModelException("nullptr block provided");
        }
    }

    bool is_virtual() const override { return false; }

    std::optional<tmdl::codegen::InterfaceDefinition> get_input_type() const override {
        if (block->get_input_num() > 0) {
            std::vector<std::string> fields;
            for (size_t i = 0; i < block->get_input_num(); ++i) {
                fields.push_back(block->get_input_name(i));
            }
            return tmdl::codegen::InterfaceDefinition("s_in", fields);
        } else {
            return {};
        }
    }

    std::optional<tmdl::codegen::InterfaceDefinition> get_output_type() const override {
        if (block->get_output_num() > 0) {
            std::vector<std::string> fields;
            for (size_t i = 0; i < block->get_output_num(); ++i) {
                fields.push_back(block->get_output_name(i));
            }
            return tmdl::codegen::InterfaceDefinition("s_out", fields);
        } else {
            return {};
        }
    }

    std::string get_name_base() const override { return block->get_block_name(); }

    std::string get_module_name() const override { return "mtstdlib.hpp"; }

    std::string get_type_name() const override { return block->get_block_name(); }

    std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override { return {}; }

    std::vector<std::string> constructor_arguments() const override { return {}; }

protected:
    std::vector<std::string> write_cpp_code(tmdl::codegen::CodeSection section) const override { return {}; }

private:
    std::unique_ptr<mt::stdlib::block_interface> block;
};

class StdlibBlockCompiled final : public tmdl::CompiledBlockInterface {
public:
    StdlibBlockCompiled(std::function<std::unique_ptr<mt::stdlib::block_interface>()> make_new_interface, size_t current_id)
        : make_new_interface(make_new_interface), current_id(current_id) {}

    std::unique_ptr<tmdl::BlockExecutionInterface> get_execution_interface(const tmdl::ConnectionManager& connections,
                                                                           const tmdl::VariableManager& manager) const override {
        // Create the block to use for execution
        auto block = make_new_interface();

        // Obtain inputs and outputs
        std::vector<std::shared_ptr<const tmdl::ModelValue>> inputs;
        for (size_t i = 0; i < block->get_input_num(); ++i) {
            inputs.push_back(manager.get_ptr(*connections.get_connection_to(current_id, i)));
        }

        std::vector<std::shared_ptr<tmdl::ModelValue>> outputs;
        for (size_t i = 0; i < block->get_output_num(); ++i) {
            outputs.push_back(manager.get_ptr(tmdl::VariableIdentifier{.block_id = current_id, .output_port_num = i}));
        }

        // Create the executor
        return std::make_unique<StdlibBlockExecutor>(std::move(block), std::move(inputs), std::move(outputs));
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override {
        return std::make_unique<StdlibBlockComponent>(make_new_interface());
    }

private:
    std::function<std::unique_ptr<mt::stdlib::block_interface>()> make_new_interface;
    size_t current_id;
};

class StdlibBlock : public tmdl::BlockInterface {
public:
    StdlibBlock(const mt::stdlib::BlockInformation& info) : constructor{info} {
        const auto init_dt = constructor.info.get_default_data_type();

        update_block(init_dt);

        if (!block) {
            throw tmdl::ModelException(fmt::format("unable to generate block for {}", info.name));
        }
    }

    std::string get_name() const override { return constructor.info.name; }

    std::string get_description() const override { return fmt::format("STDLIB BLOCK - {}", get_name()); }

    std::vector<std::shared_ptr<tmdl::Parameter>> get_parameters() const override {
        std::vector<std::shared_ptr<tmdl::Parameter>> params;

        if (constructor.param_size) {
            params.push_back(constructor.param_size);
        }

        if (constructor.param_dt) {
            params.push_back(constructor.param_dt);
        }

        if (constructor.param_value) {
            params.push_back(constructor.param_value);
        }

        return params;
    }

    mt::stdlib::DataType selected_type() const {
        if (constructor.param_dt != nullptr) {
            return constructor.param_dt->get_type();
        } else if (block != nullptr) {
            return block->get_current_type();
        } else {
            return constructor.info.get_default_data_type();
        }
    }

    bool update_block() override { return update_block(selected_type()); }

    bool update_block(tmdl::DataType new_dtype) {
        // Create the new block
        std::unique_ptr<mt::stdlib::block_interface> new_block = nullptr;
        try {
            new_block = constructor.create_block(new_dtype, 0.0);
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
        if (constructor.param_dt != nullptr && constructor.param_dt->get_type() != block->get_current_type()) {
            return make_error(fmt::format("unsupported type provided - {} != {}", get_meta_type_name(constructor.param_dt->get_type()),
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
                if (constructor.param_dt == nullptr && block->get_input_type_settable(port) && constructor.info.type_supported(type) &&
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
        const auto dt = s.get_dt();
        const auto dtype = selected_type();
        const auto c = constructor;
        return std::make_unique<StdlibBlockCompiled>([c, dt, dtype]() { return c.create_block(dtype, dt); }, get_id());
    }

private:
    const StdlibBlockConstructor constructor;
    std::unique_ptr<mt::stdlib::block_interface> block{nullptr};
    std::vector<mt::stdlib::DataType> input_types;
};

tmdl::blocks::StandardLibrary::StandardLibrary() {
    block_map = {{"input", []() { return std::make_unique<InputPort>(); }}, {"output", []() { return std::make_unique<OutputPort>(); }}};

    for (const auto& blk : mt::stdlib::get_available_blocks()) {
        block_map[blk.name] = [blk]() { return std::make_unique<StdlibBlock>(blk); };
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
