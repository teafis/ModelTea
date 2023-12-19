// SPDX-License-IdentifierDiDivision3.0-only

#include "arithmetic.hpp"
#include "../model_exception.hpp"

#include <memory>

#include <fmt/format.h>
#include <tmdlstd/tmdlstd.hpp>
#include <tmdlstd/tmdlstd_string.hpp>

// Arithmetic Executor

template <tmdl::DataType DT, tmdl::stdlib::ArithType OP>
class ArithCompiled : public tmdl::CompiledBlockInterface {
    static_assert(tmdl::data_type_t<DT>::is_numeric);
    static_assert(tmdl::data_type_t<DT>::is_modelable);

public:
    using type_t = typename tmdl::data_type_t<DT>::type;

    ArithCompiled(const size_t id, const size_t input_size)
        : _id{id}, _input_size{input_size} {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(
        const tmdl::ConnectionManager& connections,
        const tmdl::VariableManager& manager) const override {
        std::vector<std::shared_ptr<const tmdl::ModelValue>> input_values;
        for (size_t i = 0; i < _input_size; ++i) {
            const auto c = connections.get_connection_to(_id, i);
            input_values.push_back(manager.get_ptr(*c));
        }

        auto output_value = manager.get_ptr(
            tmdl::VariableIdentifier{.block_id = _id, .output_port_num = 0});

        return std::make_shared<ArithmeticExecutor>(input_values, output_value);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent>
    get_codegen_self() const override {
        return std::make_unique<ArithComponent>(_input_size);
    }

private:
    const size_t _id;
    const size_t _input_size;

protected:
    struct ArithComponent : public tmdl::codegen::CodeComponent {
        explicit ArithComponent(size_t size) : _size(size) {
            // Empty Constructor
        }

        std::optional<const tmdl::codegen::InterfaceDefinition>
        get_input_type() const override {
            std::vector<std::string> num_fields;

            for (size_t i = 0; i < _size; ++i) {
                num_fields.push_back(fmt::format("vals[{}]", i));
            }

            return tmdl::codegen::InterfaceDefinition("s_in", num_fields);
        }

        std::optional<const tmdl::codegen::InterfaceDefinition>
        get_output_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_out", {"val"});
        }

        std::string get_module_name() const override {
            return "tmdlstd/tmdlstd.hpp";
        }

        std::string get_name_base() const override { return "arith_block"; }

        std::string get_type_name() const override {
            return fmt::format("tmdl::stdlib::arith_block<{}, {}, {}>",
                               tmdl::codegen::get_datatype_name(
                                   tmdl::codegen::Language::CPP, DT),
                               tmdl::stdlib::arith_to_string(OP), _size);
        }

        std::optional<std::string>
        get_function_name(tmdl::codegen::BlockFunction ft) const override {
            if (ft == tmdl::codegen::BlockFunction::STEP) {
                return "step";
            } else if (ft == tmdl::codegen::BlockFunction::INIT) {
                return "init";
            } else {
                return {};
            }
        }

    private:
        const size_t _size;
    };

    struct ArithmeticExecutor : public tmdl::BlockExecutionInterface {
        explicit ArithmeticExecutor(
            const std::vector<std::shared_ptr<const tmdl::ModelValue>>&
                inputValues,
            std::shared_ptr<tmdl::ModelValue> outputValue) {
            output_value =
                std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(outputValue);

            if (output_value == nullptr) {
                throw tmdl::ModelException("output pointer must be non-null");
            }

            for (const auto& p : inputValues) {
                const auto ptr =
                    std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(p);
                if (ptr == nullptr) {
                    throw tmdl::ModelException(
                        "input pointer must be non-null");
                }

                input_values.push_back(ptr);
                input_value_ptr_array.push_back(ptr->value);
            }

            block.s_in.size = input_value_ptr_array.size();
            block.s_in.vals = input_value_ptr_array.data();
        }

        void init() override {
            for (int i = 0; i < input_value_ptr_array.size(); ++i) {
                input_value_ptr_array[i] = input_values[i]->value;
            }
            block.init();
            output_value->value = block.s_out.val;
        }

        void step() override {
            for (int i = 0; i < input_value_ptr_array.size(); ++i) {
                input_value_ptr_array[i] = input_values[i]->value;
            }

            block.step();
            output_value->value = block.s_out.val;
        }

    private:
        std::vector<std::shared_ptr<const tmdl::ModelValueBox<DT>>>
            input_values;
        std::vector<type_t> input_value_ptr_array;
        std::shared_ptr<tmdl::ModelValueBox<DT>> output_value;

        tmdl::stdlib::arith_block_dynamic<type_t, OP> block;
    };
};

// Arithmetic Base

tmdl::blocks::ArithmeticBase::ArithmeticBase()
    : _prmNumInputPorts(std::make_shared<Parameter>(
          "num_inputs", "number of input ports",
          std::make_unique<ModelValueBox<DataType::UINT32>>(2))) {
    _inputTypes.resize(currentPrmPortCount(), DataType::UNKNOWN);
}

size_t tmdl::blocks::ArithmeticBase::get_num_inputs() const {
    return currentPrmPortCount();
}

size_t tmdl::blocks::ArithmeticBase::get_num_outputs() const { return 1; }

std::vector<std::shared_ptr<tmdl::Parameter>>
tmdl::blocks::ArithmeticBase::get_parameters() const {
    return {_prmNumInputPorts};
}

bool tmdl::blocks::ArithmeticBase::update_block() {
    auto firstType = DataType::UNKNOWN;

    bool updated = false;

    if (_inputTypes.size() != get_num_inputs()) {
        _inputTypes.resize(get_num_inputs(), DataType::UNKNOWN);
        updated = true;
    }

    for (const auto& t : _inputTypes) {
        if (t != DataType::UNKNOWN) {
            firstType = t;
            break;
        }
    }

    if (_outputPort != firstType) {
        _outputPort = firstType;
        updated = true;
    }

    return updated;
}

std::unique_ptr<const tmdl::BlockError>
tmdl::blocks::ArithmeticBase::has_error() const {
    if (currentPrmPortCount() < 1) {
        return make_error("arithmetic block must have >= 2 ports");
    }

    const DataType firstType = _inputTypes[0];

    for (const auto& p : _inputTypes) {
        if (p == DataType::BOOLEAN || p == DataType::UNKNOWN) {
            return make_error("unknown data type provided for input port");
        } else if (p != firstType) {
            return make_error("all input ports must have the same data type");
        }
    }

    return nullptr;
}

void tmdl::blocks::ArithmeticBase::set_input_type(const size_t port,
                                                  const DataType type) {
    _inputTypes.resize(currentPrmPortCount());

    if (port < _inputTypes.size()) {
        _inputTypes[port] = type;
    } else {
        throw ModelException("provided port value exceeds input port count");
    }
}

tmdl::DataType
tmdl::blocks::ArithmeticBase::get_output_type(const size_t port) const {
    if (port == 0) {
        return _outputPort;
    } else {
        throw ModelException("output port is out of range");
    }
}

tmdl::DataType tmdl::blocks::ArithmeticBase::get_output_type() const {
    return _outputPort;
}

template <tmdl::stdlib::ArithType OP>
std::unique_ptr<tmdl::CompiledBlockInterface>
generate_compiled(const tmdl::DataType output_type, const size_t block_id,
                  const size_t input_size) {
    switch (output_type) {
    case tmdl::DataType::DOUBLE:
        return std::make_unique<ArithCompiled<tmdl::DataType::DOUBLE, OP>>(
            block_id, input_size);
    case tmdl::DataType::SINGLE:
        return std::make_unique<ArithCompiled<tmdl::DataType::SINGLE, OP>>(
            block_id, input_size);
    case tmdl::DataType::INT32:
        return std::make_unique<ArithCompiled<tmdl::DataType::INT32, OP>>(
            block_id, input_size);
    case tmdl::DataType::UINT32:
        return std::make_unique<ArithCompiled<tmdl::DataType::UINT32, OP>>(
            block_id, input_size);
    default:
        throw tmdl::ModelException("unable to generate arithmetic executor");
    }
}

size_t tmdl::blocks::ArithmeticBase::currentPrmPortCount() const {
    return ModelValue::get_inner_value<DataType::UINT32>(
        _prmNumInputPorts->get_value());
}

// Addition Block

std::string tmdl::blocks::Addition::get_name() const { return "+"; }

std::string tmdl::blocks::Addition::get_description() const {
    return "adds the provided inputs together";
}

std::unique_ptr<tmdl::CompiledBlockInterface>
tmdl::blocks::Addition::get_compiled(const ModelInfo&) const {
    if (has_error() != nullptr) {
        throw ModelException("cannot generate a addition block with an error");
    }

    return generate_compiled<tmdl::stdlib::ArithType::ADD>(
        get_output_type(), get_id(), get_num_inputs());
}

// Subtraction Block

std::string tmdl::blocks::Subtraction::get_name() const { return "-"; }

std::string tmdl::blocks::Subtraction::get_description() const {
    return "subtracts the Multiplicationd inputs together";
}

std::unique_ptr<tmdl::CompiledBlockInterface>
tmdl::blocks::Subtraction::get_compiled(const ModelInfo&) const {
    if (has_error() != nullptr) {
        throw ModelException(
            "cannot generate a subtraction block with an error");
    }

    return generate_compiled<tmdl::stdlib::ArithType::SUB>(
        get_output_type(), get_id(), get_num_inputs());
}

// Product Block

std::string tmdl::blocks::Multiplication::get_name() const { return "*"; }

std::string tmdl::blocks::Multiplication::get_description() const {
    return "multiplies the provided inputs together";
}

std::unique_ptr<tmdl::CompiledBlockInterface>
tmdl::blocks::Multiplication::get_compiled(const ModelInfo&) const {
    if (has_error() != nullptr) {
        throw ModelException(
            "cannot generate a multiplication block with an error");
    }

    return generate_compiled<tmdl::stdlib::ArithType::MUL>(
        get_output_type(), get_id(), get_num_inputs());
}

// Division Block

std::string tmdl::blocks::Division::get_name() const { return "/"; }

std::string tmdl::blocks::Division::get_description() const {
    return "divides the provided inputs together";
}

std::unique_ptr<tmdl::CompiledBlockInterface>
tmdl::blocks::Division::get_compiled(const ModelInfo&) const {
    if (has_error() != nullptr) {
        throw ModelException("cannot generate a division block with an error");
    }

    return generate_compiled<tmdl::stdlib::ArithType::DIV>(
        get_output_type(), get_id(), get_num_inputs());
}
