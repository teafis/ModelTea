// SPDX-License-Identifier: GPL-3.0-only

#include "clock.hpp"
#include "../model_exception.hpp"

#include <memory>

#include <fmt/format.h>

#include <mtstd.hpp>

class CompiledClock : public tmdl::CompiledBlockInterface {
public:
    CompiledClock(const size_t id, const tmdl::BlockInterface::ModelInfo& s) : _id(id), _state(s) {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(const tmdl::ConnectionManager&,
                                                                           const tmdl::VariableManager& manager) const override {
        tmdl::VariableIdentifier vid = {.block_id = _id, .output_port_num = 0};

        auto ptr = std::dynamic_pointer_cast<tmdl::ModelValueBox<tmdl::DataType::DOUBLE>>(manager.get_ptr(vid));

        return std::make_shared<ClockExecutor>(ptr, _state);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override { return std::make_unique<ClockComponent>(_state); }

private:
    const size_t _id;
    const tmdl::BlockInterface::ModelInfo _state;

protected:
    struct ClockComponent : public tmdl::codegen::CodeComponent {
        ClockComponent(const tmdl::BlockInterface::ModelInfo& s) : _state(s) {
            // Empty Constructor
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override { return {}; }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_out", {"val"});
        }

        std::string get_module_name() const override { return "tmdlstd/tmdlstd.hpp"; }

        std::string get_name_base() const override { return "clock_block"; }

        std::string get_type_name() const override { return "mt::stdlib::clock_block"; }

        std::vector<std::string> constructor_arguments() const override { return {std::to_string(_state.get_dt())}; }

        std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override {
            if (ft == tmdl::codegen::BlockFunction::STEP) {
                return "step";
            } else if (ft == tmdl::codegen::BlockFunction::RESET) {
                return "reset";
            } else {
                return {};
            }
        }

        const tmdl::BlockInterface::ModelInfo _state;
    };

    class ClockExecutor : public tmdl::BlockExecutionInterface {
    public:
        ClockExecutor(std::shared_ptr<tmdl::ModelValueBox<tmdl::DataType::DOUBLE>> ptr_output, const tmdl::BlockInterface::ModelInfo& s)
            : output_value(ptr_output), block(s.get_dt()), state(s) {
            if (ptr_output == nullptr) {
                throw tmdl::ModelException("input pointers cannot be null");
            }
        }

    protected:
        void blk_reset() override { block.reset(); }

        void blk_step() override { block.step(); }

        void update_inputs() override {}
        void update_outputs() override { output_value->value = block.s_out.val; }

    private:
        std::shared_ptr<tmdl::ModelValueBox<tmdl::DataType::DOUBLE>> output_value;
        mt::stdlib::clock_block block;
        const tmdl::BlockInterface::ModelInfo state;
    };
};

std::string tmdl::blocks::Clock::get_name() const { return "clock"; }

std::string tmdl::blocks::Clock::get_description() const { return "Clock to provide the current time in seconds"; }

size_t tmdl::blocks::Clock::get_num_inputs() const { return 0; }

size_t tmdl::blocks::Clock::get_num_outputs() const { return 1; }

bool tmdl::blocks::Clock::update_block() { return false; }

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::Clock::has_error() const { return nullptr; }

void tmdl::blocks::Clock::set_input_type(const size_t, const DataType) { throw ModelException("no input ports for clock"); }

tmdl::DataType tmdl::blocks::Clock::get_output_type(const size_t port) const {
    if (port == 0) {
        return DataType::DOUBLE;
    } else {
        throw ModelException("invalid output port requested");
    }
}

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::Clock::get_compiled(const ModelInfo& s) const {
    return std::make_unique<CompiledClock>(get_id(), s);
}
