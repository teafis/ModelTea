// SPDX-License-Identifier: GPL-3.0-only

#include "block_io_ports.hpp"

#include <fmt/format.h>

#include "model_exception.hpp"
#include "block_interface.hpp"

using namespace mtea;

/* ========== COMPILED STRUCTURE ========== */

class IoPortComponent : public mtea::codegen::CodeComponent {
public:
    bool is_virtual() const override { return true; }

    std::string get_name_base() const override { return "io_port"; }

    std::optional<mtea::codegen::InterfaceDefinition> get_input_type() const override {
        throw mtea::codegen::CodegenError(fmt::format("unable to generate code for {}", get_name_base()));
    }

    std::optional<mtea::codegen::InterfaceDefinition> get_output_type() const override {
        throw mtea::codegen::CodegenError(fmt::format("unable to generate code for {}", get_name_base()));
    }

    std::optional<std::string> get_function_name(mtea::codegen::BlockFunction) const override {
        throw mtea::codegen::CodegenError(fmt::format("unable to generate code for {}", get_name_base()));
    }

protected:
    std::vector<std::string> write_code(mtea::codegen::CodeSection) const override {
        throw mtea::codegen::CodegenError(fmt::format("unable to generate code for {}", get_name_base()));
    }
};

class CompiledPort : public mtea::CompiledBlockInterface {
    class PortExecutor : public mtea::BlockExecutionInterface {
    protected:
        void update_inputs() override {}
        void update_outputs() override {}
    };

public:
    std::unique_ptr<BlockExecutionInterface> get_execution_interface(const ConnectionManager&, const VariableManager&) const override {
        return std::make_unique<PortExecutor>();
    }

    std::unique_ptr<codegen::CodeComponent> get_codegen_self() const override { return std::make_unique<IoPortComponent>(); }
};

/* ========== INPUT PORT ========== */

InputPort::InputPort(std::string_view library) : mtea::BlockInterface(library) {
    dataTypeParameter = std::make_shared<ParameterDataType>("data_type", "parameter data type", DataType::NONE);
}

std::string InputPort::get_name() const { return "input"; }

std::string InputPort::get_description() const { return "Input Port"; }

size_t InputPort::get_num_inputs() const { return 0; }

size_t InputPort::get_num_outputs() const { return 1; }

void InputPort::set_input_type(const size_t, const DataType) { throw ModelException("cannot set input port value"); }

DataType InputPort::get_output_type(const size_t port) const {
    if (port == 0) {
        return _port;
    } else {
        throw ModelException("cannot provide output for provided port number");
    }
}

std::unique_ptr<const BlockError> InputPort::has_error() const {
    if (_port == DataType::NONE) {
        return make_error("input port has unknown type");
    }

    return nullptr;
}

std::vector<std::shared_ptr<mtea::Parameter>> InputPort::get_parameters() const { return {dataTypeParameter}; }

bool InputPort::update_block() {
    if (const auto param_dt = get_output_type(); param_dt != _port) {
        _port = param_dt;
        return true;
    }

    return false;
}

std::unique_ptr<CompiledBlockInterface> InputPort::get_compiled(const ModelInfo&) const { return std::make_unique<CompiledPort>(); }

void InputPort::set_input_value(const DataType type) { _port = type; }

mtea::DataType InputPort::get_output_type() const { return dataTypeParameter->get_type(); }

/* ========== OUTPUT PORT ========== */

OutputPort::OutputPort(std::string_view library) : mtea::BlockInterface(library) {}

std::string OutputPort::get_name() const { return "output"; }

std::string OutputPort::get_description() const { return "Output Port"; }

size_t OutputPort::get_num_inputs() const { return 1; }

size_t OutputPort::get_num_outputs() const { return 0; }

void OutputPort::set_input_type(const size_t port, const DataType type) {
    if (port == 0) {
        _port = type;
    } else {
        throw ModelException("cannot set input for provided port number");
    }
}

DataType OutputPort::get_output_type(const size_t /* port */) const { throw ModelException("cannot get input port value"); }

std::unique_ptr<const BlockError> OutputPort::has_error() const {
    if (_port == DataType::NONE) {
        return make_error("output port has unknown type");
    }

    return nullptr;
}

bool OutputPort::update_block() { return false; }

std::unique_ptr<CompiledBlockInterface> OutputPort::get_compiled(const ModelInfo&) const { return std::make_unique<CompiledPort>(); }

DataType OutputPort::get_output_value() const { return _port; }
