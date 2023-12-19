// SPDX-License-Identifier: GPL-3.0-only

#include "delay.hpp"
#include "../model_exception.hpp"

#include <fmt/format.h>

#include <tmdlstd/tmdlstd.hpp>

#include <memory>

template <tmdl::DataType DT> class CompiledDelay : public tmdl::CompiledBlockInterface {
    static_assert(tmdl::data_type_t<DT>::is_modelable);

public:
    CompiledDelay(const size_t id) : _id{id} {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(const tmdl::ConnectionManager& connections,
                                                                           const tmdl::VariableManager& manager) const override {
        auto input_value = manager.get_ptr(*connections.get_connection_to(_id, 0));

        auto input_value_reset_flag = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>>(
            manager.get_ptr(*connections.get_connection_to(_id, 1)));
        auto input_value_reset_value = manager.get_ptr(*connections.get_connection_to(_id, 2));

        auto output_value = manager.get_ptr(tmdl::VariableIdentifier{.block_id = _id, .output_port_num = 0});

        return std::make_shared<DelayExecutor>(input_value, output_value, input_value_reset_flag, input_value_reset_value);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override { return std::make_unique<DelayComponent>(); }

private:
    const size_t _id;

protected:
    struct DelayComponent : public tmdl::codegen::CodeComponent {
        std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_out", {"input", "reset_flag", "reset"});
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_out", {"value"});
        }

        std::string get_module_name() const override { return "tmdlstd/tmdlstd.hpp"; }

        std::string get_name_base() const override { return "delay_block"; }

        std::string get_type_name() const override {
            return fmt::format("tmdl::stdlib::delay_block<{}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT));
        }

        std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override {
            switch (ft) {
                using enum tmdl::codegen::BlockFunction;
            case RESET:
                return "reset";
            case STEP:
                return "step";
            default:
                return {};
            }
        }
    };

    class DelayExecutor : public tmdl::BlockExecutionInterface {
    public:
        using type_t = typename tmdl::data_type_t<DT>::type;

        explicit DelayExecutor(const std::shared_ptr<const tmdl::ModelValue> input, const std::shared_ptr<tmdl::ModelValue> output,
                               const std::shared_ptr<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> reset_flag,
                               const std::shared_ptr<const tmdl::ModelValue> reset_value)
            : _input(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(input)),
              _output(std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(output)), _reset_flag(reset_flag),
              _reset_value(std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(reset_value)) {
            if (_input == nullptr || _output == nullptr || _reset_flag == nullptr || _reset_value == nullptr) {
                throw tmdl::ModelException("input values must be non-null");
            }
        }

    protected:
        void update_inputs() override {
            block.s_in.value = _input->value;
            block.s_in.reset = _reset_value->value;
            block.s_in.reset_flag = _reset_flag->value;
        }

        void update_outputs() override { _output->value = block.s_out.value; }

        void blk_reset() override { block.reset(); }

        void blk_step() override { block.step(); }

    private:
        tmdl::stdlib::delay_block<type_t> block;

        std::shared_ptr<const tmdl::ModelValueBox<DT>> _input;
        std::shared_ptr<tmdl::ModelValueBox<DT>> _output;
        std::shared_ptr<const tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> _reset_flag;
        std::shared_ptr<const tmdl::ModelValueBox<DT>> _reset_value;
    };
};

tmdl::blocks::Delay::Delay() {
    using enum DataType;
    input_type = UNKNOWN;
    input_reset_flag = UNKNOWN;
    input_reset_value = UNKNOWN;
    output_port = UNKNOWN;
}

std::string tmdl::blocks::Delay::get_name() const { return "delay"; }

std::string tmdl::blocks::Delay::get_description() const { return "delays the provided input value by one cycle"; }

size_t tmdl::blocks::Delay::get_num_inputs() const { return 3; }

size_t tmdl::blocks::Delay::get_num_outputs() const { return 1; }

bool tmdl::blocks::Delay::update_block() {
    if (input_type != output_port) {
        output_port = input_type;
        return true;
    }

    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::Delay::has_error() const {
    if (input_type != output_port) {
        return make_error("input type does not equal output type");
    } else if (input_reset_flag != DataType::BOOLEAN) {
        return make_error("reset flag must be boolean");
    } else if (input_type != input_reset_value) {
        return make_error("input type must equal the reset value type");
    }

    return nullptr;
}

void tmdl::blocks::Delay::set_input_type(const size_t port, const DataType type) {
    switch (port) {
    case 0:
        input_type = type;
        break;
    case 1:
        input_reset_flag = type;
        break;
    case 2:
        input_reset_value = type;
        break;
    default:
        throw ModelException("invalid input port provided");
    }
}

tmdl::DataType tmdl::blocks::Delay::get_output_type(const size_t port) const {
    if (port == 0) {
        return output_port;
    } else {
        throw ModelException("invalid output port provided");
    }
}

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::Delay::get_compiled(const ModelInfo&) const {
    if (has_error() != nullptr) {
        throw ModelException("cannot build component with error");
    }

    const auto _id_val = get_id();

    switch (output_port) {
        using enum DataType;
    case DOUBLE:
        return std::make_unique<CompiledDelay<DOUBLE>>(_id_val);
    case SINGLE:
        return std::make_unique<CompiledDelay<SINGLE>>(_id_val);
    case BOOLEAN:
        return std::make_unique<CompiledDelay<BOOLEAN>>(_id_val);
    case INT32:
        return std::make_unique<CompiledDelay<INT32>>(_id_val);
    case UINT32:
        return std::make_unique<CompiledDelay<UINT32>>(_id_val);
    default:
        throw ModelException("unknown data type provided");
    }
}
