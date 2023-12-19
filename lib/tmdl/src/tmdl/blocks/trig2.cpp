// SPDX-License-Identifier: GPL-3.0-only

#include "trig2.hpp"

#include "../model_exception.hpp"
#include "tmdl/values/data_types.hpp"

#include <tmdlstd/tmdlstd.hpp>
#include <tmdlstd/tmdlstd_string.hpp>

#include <fmt/format.h>

tmdl::blocks::TrigFunction2::TrigFunction2() {
    input_type_1 = DataType::UNKNOWN;
    input_type_2 = DataType::UNKNOWN;
    output_port = DataType::UNKNOWN;
}

size_t tmdl::blocks::TrigFunction2::get_num_inputs() const { return 2; }

size_t tmdl::blocks::TrigFunction2::get_num_outputs() const { return 1; }

bool tmdl::blocks::TrigFunction2::update_block() {
    if (input_type_1 != output_port) {
        output_port = input_type_1;
        return true;
    } else {
        return false;
    }
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::TrigFunction2::has_error() const {
    if (input_type_1 != output_port) {
        return make_error("mismatch in input and output types");
    } else if (input_type_1 != DataType::DOUBLE && input_type_1 != DataType::SINGLE) {
        return make_error("invalid input port type provided");
    } else if (input_type_1 != input_type_2) {
        return make_error("input ports must have the same data type");
    }

    return nullptr;
}

void tmdl::blocks::TrigFunction2::set_input_type(const size_t port, const DataType type) {
    if (port == 0) {
        input_type_1 = type;
    } else if (port == 1) {
        input_type_2 = type;
    } else {
        throw ModelException("invalid input port provided");
    }
}

tmdl::DataType tmdl::blocks::TrigFunction2::get_output_type(const size_t port) const {
    if (port == 0) {
        return output_port;
    } else {
        throw ModelException("invalid output port provided");
    }
}

template <tmdl::DataType DT, tmdl::stdlib::TrigFunction2 FCN> class CompiledTrig : public tmdl::CompiledBlockInterface {
public:
    explicit CompiledTrig(const size_t id) : _id{id} {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(const tmdl::ConnectionManager& connections,
                                                                           const tmdl::VariableManager& manager) const override {
        const auto inputValue1 = manager.get_ptr(*connections.get_connection_to(_id, 0));
        const auto inputValue2 = manager.get_ptr(*connections.get_connection_to(_id, 1));
        const auto outputValue = manager.get_ptr(tmdl::VariableIdentifier{.block_id = _id, .output_port_num = 0});

        return std::make_shared<TrigExecutor>(inputValue1, inputValue2, outputValue);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override { return std::make_unique<TrigComponent>(); }

private:
    const size_t _id;

protected:
    struct TrigComponent : public tmdl::codegen::CodeComponent {
        std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_in", {"value"});
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
            case tmdl::codegen::BlockFunction::INIT:
                return "init";
            default:
                return {};
            }
        }
    };

    class TrigExecutor : public tmdl::BlockExecutionInterface {
    public:
        using val_t = typename tmdl::data_type_t<DT>::type;

        TrigExecutor(std::shared_ptr<const tmdl::ModelValue> ptr_input_1, std::shared_ptr<const tmdl::ModelValue> ptr_input_2,
                     std::shared_ptr<tmdl::ModelValue> ptr_output)
            : _ptr_input_1(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(ptr_input_1)),
              _ptr_input_2(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(ptr_input_2)),
              _ptr_output(std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(ptr_output)) {
            if (_ptr_input_1 == nullptr || _ptr_output == nullptr) {
                throw tmdl::ModelException("input pointers cannot be null");
            }
        }

    protected:
        void update_inputs() override {
            block.s_in.value_a = _ptr_input_1->value;
            block.s_in.value_b = _ptr_input_2->value;
        }

        void update_outputs() override { _ptr_output->value = block.s_out.value; }

        void blk_init() override { block.init(); }

        void blk_step() override { block.step(); }

    private:
        const std::shared_ptr<const tmdl::ModelValueBox<DT>> _ptr_input_1;
        const std::shared_ptr<const tmdl::ModelValueBox<DT>> _ptr_input_2;
        const std::shared_ptr<tmdl::ModelValueBox<DT>> _ptr_output;
        tmdl::stdlib::trig_block_2<val_t, FCN> block;
    };
};

template <tmdl::stdlib::TrigFunction2 FCN>
static std::unique_ptr<tmdl::CompiledBlockInterface> generate_compiler_interface(const tmdl::blocks::TrigFunction2* model) {
    if (model->has_error() != nullptr) {
        throw tmdl::ModelException(fmt::format("cannot execute {} with incomplete input parameters", model->get_name()));
    }

    switch (model->get_output_type(0)) {
    case tmdl::DataType::DOUBLE:
        return std::make_unique<CompiledTrig<tmdl::DataType::DOUBLE, FCN>>(model->get_id());
    case tmdl::DataType::SINGLE:
        return std::make_unique<CompiledTrig<tmdl::DataType::SINGLE, FCN>>(model->get_id());
    default:
        throw tmdl::ModelException("unable to generate limitor executor");
    }
}

std::string tmdl::blocks::TrigATan2::get_name() const { return "atan2"; }

std::string tmdl::blocks::TrigATan2::get_description() const {
    return "computes the arctangent of the input parameter, with the first "
           "input being y and the secont input being x, for atan(y/x)";
}

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::TrigATan2::get_compiled(const ModelInfo&) const {
    return generate_compiler_interface<tmdl::stdlib::TrigFunction2::ATAN2>(this);
}
