// SPDX-License-Identifier: GPL-3.0-only

#include "trig.hpp"

#include "../model_exception.hpp"
#include "tmdl/values/value.hpp"

#include <ranges>
#include <tmdlstd/tmdlstd.hpp>
#include <tmdlstd/tmdlstd_string.hpp>

#include <fmt/format.h>

tmdl::blocks::TrigFunction::TrigFunction(const size_t num_inputs) {
    input_types.resize(num_inputs, DataType::UNKNOWN);
    if (num_inputs < 1) {
        throw ModelException(fmt::format("expected at least one input value, found {}", num_inputs));
    }
}

size_t tmdl::blocks::TrigFunction::get_num_inputs() const { return input_types.size(); }

size_t tmdl::blocks::TrigFunction::get_num_outputs() const { return 1; }

bool tmdl::blocks::TrigFunction::update_block() {
    if (input_types.front() != output_port) {
        output_port = input_types.front();
        return true;
    } else {
        return false;
    }
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::TrigFunction::has_error() const {
    const auto first_input = input_types.front();

    if (first_input != output_port) {
        return make_error("mismatch in input and output types");
    } else if (first_input != DataType::DOUBLE && first_input != DataType::SINGLE) {
        return make_error("invalid input port type provided");
    }

    for (const auto& t : input_types | std::views::drop(1)) {
        if (t != first_input) {
            return make_error("unexpected data type does not match first data type");
        }
    }

    return nullptr;
}

void tmdl::blocks::TrigFunction::set_input_type(const size_t port, const DataType type) {
    if (port < input_types.size()) {
        input_types[port] = type;
    } else {
        throw ModelException("invalid input port provided");
    }
}

tmdl::DataType tmdl::blocks::TrigFunction::get_output_type(const size_t port) const {
    if (port == 0) {
        return output_port;
    } else {
        throw ModelException("invalid output port provided");
    }
}

template <tmdl::DataType DT, tmdl::stdlib::TrigFunction FCN> class CompiledTrig : public tmdl::CompiledBlockInterface {
public:
    explicit CompiledTrig(const size_t id, const size_t num_inputs) : _id{id}, _num_inputs{num_inputs} {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(const tmdl::ConnectionManager& connections,
                                                                           const tmdl::VariableManager& manager) const override {
        std::vector<std::shared_ptr<const tmdl::ModelValue>> inputValues;
        for (size_t i = 0; i < _num_inputs; ++i) {
            inputValues.push_back(manager.get_ptr(*connections.get_connection_to(_id, i)));
        }

        const auto outputValue = manager.get_ptr(tmdl::VariableIdentifier{.block_id = _id, .output_port_num = 0});

        return std::make_shared<TrigExecutor>(inputValues, outputValue);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override { return std::make_unique<TrigComponent>(_num_inputs); }

private:
    const size_t _id;
    const size_t _num_inputs;

protected:
    struct TrigComponent : public tmdl::codegen::CodeComponent {
        TrigComponent(const size_t num_inputs) : _num_inputs{num_inputs} {}

        std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override {
            std::vector<std::string> input_fields;
            for (size_t i = 0; i < _num_inputs; ++i) {
                input_fields.push_back(fmt::format("values[{}]", i));
            }

            return tmdl::codegen::InterfaceDefinition("s_in", input_fields);
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_out", {"value"});
        }

        std::string get_module_name() const override { return "tmdlstd/tmdlstd.hpp"; }

        std::string get_name_base() const override { return "trig_block"; }

        std::string get_type_name() const override {
            return fmt::format("tmdl::stdlib::trig_block<{}, {}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT),
                               tmdl::stdlib::trig_func_to_string(FCN));
        }

        std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override {
            switch (ft) {
            case tmdl::codegen::BlockFunction::STEP:
                return "step";
            case tmdl::codegen::BlockFunction::RESET:
                return "reset";
            default:
                return {};
            }
        }

        const size_t _num_inputs;
    };

    class TrigExecutor : public tmdl::BlockExecutionInterface {
    public:
        using val_t = typename tmdl::data_type_t<DT>::type;

        TrigExecutor(std::vector<std::shared_ptr<const tmdl::ModelValue>> ptr_inputs, std::shared_ptr<tmdl::ModelValue> ptr_output)
            : _ptr_output(std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(ptr_output)) {
            _ptr_inputs.clear();

            for (const auto& p : ptr_inputs) {
                _ptr_inputs.push_back(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(p));
            }

            for (const auto& p : _ptr_inputs) {
                if (p == nullptr) {
                    throw tmdl::ModelException("input pointer cannot be null");
                }
            }

            if (_ptr_output == nullptr) {
                throw tmdl::ModelException("output pointer cannot be null");
            }
        }

    protected:
        void update_inputs() override {
            if (_ptr_inputs.size() != tmdl::stdlib::TrigInfo<FCN>::input_count) {
                throw tmdl::ModelException("input input_size_mismatch!");
            }

            for (size_t i = 0; i < _ptr_inputs.size(); ++i) {
                block.s_in.values[i] = _ptr_inputs[i]->value;
            }
        }

        void update_outputs() override { _ptr_output->value = block.s_out.value; }

        void blk_reset() override { block.reset(); }

        void blk_step() override { block.step(); }

    private:
        std::vector<std::shared_ptr<const tmdl::ModelValueBox<DT>>> _ptr_inputs;
        const std::shared_ptr<tmdl::ModelValueBox<DT>> _ptr_output;
        tmdl::stdlib::trig_block<val_t, FCN> block;
    };
};

template <tmdl::stdlib::TrigFunction FCN>
static std::unique_ptr<tmdl::CompiledBlockInterface> generate_compiler_interface(const tmdl::blocks::TrigFunction* model) {
    if (model->has_error() != nullptr) {
        throw tmdl::ModelException(fmt::format("cannot execute {} with incomplete input parameters", model->get_name()));
    }

    switch (model->get_output_type(0)) {
    case tmdl::DataType::DOUBLE:
        return std::make_unique<CompiledTrig<tmdl::DataType::DOUBLE, FCN>>(model->get_id(), model->get_num_inputs());
    case tmdl::DataType::SINGLE:
        return std::make_unique<CompiledTrig<tmdl::DataType::SINGLE, FCN>>(model->get_id(), model->get_num_inputs());
    default:
        throw tmdl::ModelException("unable to generate limitor executor");
    }
}

#define MAKE_TRIG1_BLOCK(CLASS_NAME, TYPE_CODE, NAME)                                                                                      \
    tmdl::blocks::CLASS_NAME::CLASS_NAME() : TrigFunction(tmdl::stdlib::TrigInfo<TYPE_CODE>::input_count) {}                               \
    std::string tmdl::blocks::CLASS_NAME::get_name() const {                                                                               \
        return tmdl::stdlib::trig_func_to_string((TYPE_CODE), tmdl::stdlib::SpecificationType::NONE);                                      \
    }                                                                                                                                      \
    std::string tmdl::blocks::CLASS_NAME::get_description() const { return fmt::format("computes {} for the input parameter", (NAME)); }   \
    std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::CLASS_NAME::get_compiled(const ModelInfo&) const {                         \
        return generate_compiler_interface<(TYPE_CODE)>(this);                                                                             \
    }

MAKE_TRIG1_BLOCK(TrigSin, tmdl::stdlib::TrigFunction::SIN, "sine")
MAKE_TRIG1_BLOCK(TrigCos, tmdl::stdlib::TrigFunction::COS, "cosine")
MAKE_TRIG1_BLOCK(TrigTan, tmdl::stdlib::TrigFunction::TAN, "tangent")
MAKE_TRIG1_BLOCK(TrigASin, tmdl::stdlib::TrigFunction::ASIN, "arcsine")
MAKE_TRIG1_BLOCK(TrigACos, tmdl::stdlib::TrigFunction::ACOS, "arccosine")
MAKE_TRIG1_BLOCK(TrigATan, tmdl::stdlib::TrigFunction::ATAN, "arctangent (atan(x))")
MAKE_TRIG1_BLOCK(TrigATan2, tmdl::stdlib::TrigFunction::ATAN2, "arctangent (atan(y = in[0], x = in[1]))")
