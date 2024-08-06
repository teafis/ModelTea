// SPDX-License-Identifier: GPL-3.0-only

module;

#include <cstdlib>
#include <cstddef>
#include <memory>
#include <vector>

#include <fmt/format.h>

export module tmdl:io_ports;

import :block_interface;
import :model_exception;

namespace tmdl {

/* ========== COMPILED STRUCTURE ========== */

class IoPortComponent : public tmdl::codegen::CodeComponent {
public:
    bool is_virtual() const override { return true; }

    std::string get_name_base() const override { return "io_port"; }

    std::optional<tmdl::codegen::InterfaceDefinition> get_input_type() const override {
        throw tmdl::codegen::CodegenError(fmt::format("unable to generate code for {}", get_name_base()));
    }

    std::optional<tmdl::codegen::InterfaceDefinition> get_output_type() const override {
        throw tmdl::codegen::CodegenError(fmt::format("unable to generate code for {}", get_name_base()));
    }

    std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction) const override {
        throw tmdl::codegen::CodegenError(fmt::format("unable to generate code for {}", get_name_base()));
    }

protected:
    std::vector<std::string> write_cpp_code(tmdl::codegen::CodeSection) const override {
        throw tmdl::codegen::CodegenError(fmt::format("unable to generate code for {}", get_name_base()));
    }
};

class CompiledPort : public tmdl::CompiledBlockInterface {
    class PortExecutor : public tmdl::BlockExecutionInterface {
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

export class InputPort final : public BlockInterface {
public:
    InputPort(std::string_view library);

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_type(const size_t port, const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::unique_ptr<const BlockError> has_error() const override;

    std::vector<std::shared_ptr<Parameter>> get_parameters() const override;

    bool update_block() override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;

    void set_input_value(const DataType type);

private:
    DataType get_output_type() const;

    DataType _port;
    std::shared_ptr<ParameterDataType> dataTypeParameter;
};

export class OutputPort final : public BlockInterface {
public:
    OutputPort(std::string_view library);

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    void set_input_type(const size_t port, const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::unique_ptr<const BlockError> has_error() const override;

    bool update_block() override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;

    DataType get_output_value() const;

private:
    DataType _port;
};

/* ========== INPUT PORT ========== */

InputPort::InputPort(std::string_view library) : tmdl::BlockInterface(library) {
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

std::vector<std::shared_ptr<tmdl::Parameter>> InputPort::get_parameters() const { return {dataTypeParameter}; }

bool InputPort::update_block() {
    if (const auto param_dt = get_output_type(); param_dt != _port) {
        _port = param_dt;
        return true;
    }

    return false;
}

std::unique_ptr<CompiledBlockInterface> InputPort::get_compiled(const ModelInfo&) const { return std::make_unique<CompiledPort>(); }

void InputPort::set_input_value(const DataType type) { _port = type; }

tmdl::DataType InputPort::get_output_type() const { return dataTypeParameter->get_type(); }

/* ========== OUTPUT PORT ========== */

OutputPort::OutputPort(std::string_view library) : tmdl::BlockInterface(library) {}

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

}
