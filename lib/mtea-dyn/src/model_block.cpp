// SPDX-License-Identifier: GPL-3.0-only

#include "model_block.hpp"

#include <functional>

#include "model_exception.hpp"

mtea::ModelBlock::ModelBlock(std::shared_ptr<Model> model, std::string_view library) : mtea::BlockInterface(library), model(model) {
    ModelBlock::update_block();
}

std::string mtea::ModelBlock::get_name() const { return model->get_name(); }

std::string mtea::ModelBlock::get_description() const { return model->get_description(); }

size_t mtea::ModelBlock::get_num_outputs() const { return model->get_num_outputs(); }

size_t mtea::ModelBlock::get_num_inputs() const { return model->get_num_inputs(); }

bool mtea::ModelBlock::update_block() {
    bool updated = model->update_block();

    if (input_types.size() != get_num_inputs()) {
        input_types.resize(get_num_inputs(), DataType::NONE);
        updated = true;
    }

    if (output_types.size() != get_num_outputs()) {
        output_types.resize(get_num_outputs(), DataType::NONE);
        updated = true;
    }

    for (size_t i = 0; i < get_num_outputs(); ++i) {
        auto& dtype = output_types[i];
        const auto model_dtype = model->get_output_datatype(i);

        if (dtype != model_dtype) {
            dtype = model_dtype;
            updated = true;
        }
    }

    return updated;
}

std::unique_ptr<const mtea::BlockError> mtea::ModelBlock::has_error() const {
    if (auto err = model->has_error())
        return err;

    const std::vector<std::tuple<const std::vector<DataType>&, std::function<DataType(const size_t)>, std::function<size_t()>>> portvec{
        {input_types, [this](const size_t port) { return model->get_input_datatype(port); }, [this]() { return get_num_inputs(); }},
        {output_types, [this](const size_t port) { return model->get_output_datatype(port); }, [this]() { return get_num_outputs(); }}};

    for (const auto& it : portvec) {
        const auto& [vec, dt_func, size_func] = it;

        if (vec.size() != size_func()) {
            return make_error("model port size mismatch");
        }

        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] != dt_func(i)) {
                return make_error("model port type mismatch");
            }
        }
    }

    return nullptr;
}

void mtea::ModelBlock::set_input_type(const size_t port, const DataType type) {
    if (input_types.size() != get_num_inputs()) {
        input_types.resize(get_num_inputs(), DataType::NONE);
    }

    if (port < input_types.size()) {
        input_types[port] = type;
    } else {
        throw ModelException("input port out of range");
    }
}

mtea::DataType mtea::ModelBlock::get_output_type(const size_t port) const {
    if (port < get_num_outputs()) {
        if (port < output_types.size()) {
            return output_types[port];
        } else {
            return DataType::NONE;
        }
    } else {
        throw ModelException("output port out of range");
    }
}

struct CompiledModelBlock : public mtea::CompiledBlockInterface {
    CompiledModelBlock(const size_t id, std::shared_ptr<const mtea::Model> model, const mtea::BlockInterface::ModelInfo& s)
        : _id(id), _model(model), _state(s) {
        // Empty Constructor
    }

    std::unique_ptr<mtea::BlockExecutionInterface> get_execution_interface(const mtea::ConnectionManager& connections,
                                                                           const mtea::VariableManager& manager) const override {
        return _model->get_execution_interface(_id, connections, manager, _state);
    }

    std::vector<std::unique_ptr<mtea::codegen::CodeComponent>> get_codegen_other() const override {
        return _model->get_all_sub_components(_state);
    }

    std::unique_ptr<mtea::codegen::CodeComponent> get_codegen_self() const override { return _model->get_codegen_component(_state); }

private:
    const size_t _id;
    std::shared_ptr<const mtea::Model> _model;
    const mtea::BlockInterface::ModelInfo& _state;
};

std::unique_ptr<mtea::CompiledBlockInterface> mtea::ModelBlock::get_compiled(const BlockInterface::ModelInfo& s) const {
    return std::make_unique<CompiledModelBlock>(get_id(), model, s);
}

std::shared_ptr<mtea::Model> mtea::ModelBlock::get_model() { return model; }

std::shared_ptr<const mtea::Model> mtea::ModelBlock::get_model() const { return model; }
