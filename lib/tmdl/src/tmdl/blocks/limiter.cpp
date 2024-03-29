// SPDX-License-Identifier: GPL-3.0-only

#include "limiter.hpp"

#include "../model_exception.hpp"

#include <algorithm>

#include <fmt/format.h>

#include <mtstd.hpp>

using namespace tmdl;
using namespace tmdl::blocks;

struct LimiterValues {
    std::shared_ptr<tmdl::ModelValue> upper;
    std::shared_ptr<tmdl::ModelValue> lower;
};

template <tmdl::DataType DT> class CompiledLimiter : public tmdl::CompiledBlockInterface {
protected:
    using limit_t = typename tmdl::data_type_t<DT>::type;

public:
    CompiledLimiter(const size_t id, std::optional<LimiterValues>&& constLimits) : _id{id}, _constantLimits{std::move(constLimits)} {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(const tmdl::ConnectionManager& connections,
                                                                           const tmdl::VariableManager& manager) const override {
        std::shared_ptr<ModelValue> maxValue;
        std::shared_ptr<ModelValue> minValue;

        if (_constantLimits) {
            maxValue = _constantLimits->upper;
            minValue = _constantLimits->lower;
        } else {
            maxValue = manager.get_ptr(*connections.get_connection_to(_id, 1));
            minValue = manager.get_ptr(*connections.get_connection_to(_id, 2));
        }

        const auto inputPointer = manager.get_ptr(*connections.get_connection_to(_id, 0));

        const auto vidOutput = VariableIdentifier{.block_id = _id, .output_port_num = 0};

        const auto outputPointer = manager.get_ptr(vidOutput);

        return std::make_shared<LimiterExecutor>(inputPointer, minValue, maxValue, outputPointer);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override {
        if (_constantLimits) {
            return std::make_unique<LimiterComponentConst>(tmdl::ModelValue::get_inner_value<DT>(_constantLimits->upper.get()),
                                                           tmdl::ModelValue::get_inner_value<DT>(_constantLimits->lower.get()));
        } else {
            return std::make_unique<LimiterComponent>();
        }
    }

private:
    const size_t _id;
    const std::optional<LimiterValues> _constantLimits;

protected:
    struct LimiterComponent : public tmdl::codegen::CodeComponent {
        std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_in", {"value", "limit_upper", "limit_lower"});
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_out", {"value"});
        }

        std::string get_module_name() const override { return "tmdlstd/tmdlstd.hpp"; }

        std::string get_name_base() const override { return "limiter_block"; }

        std::string get_type_name() const override {
            return fmt::format("mt::stdlib::limiter_block<{}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT));
        }

        std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override {
            switch (ft) {
            case tmdl::codegen::BlockFunction::RESET:
                return "reset";
            case tmdl::codegen::BlockFunction::STEP:
                return "step";
            default:
                return {};
            }
        }
    };

    struct LimiterComponentConst : public tmdl::codegen::CodeComponent {
        LimiterComponentConst(const limit_t upper, const limit_t lower) : _upper{upper}, _lower{lower} {
            // Empty Constructor
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_in", {"value"});
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_out", {"value"});
        }

        std::string get_module_name() const override { return "tmdlstd/tmdlstd.hpp"; }

        std::string get_name_base() const override { return "limiter_block_const"; }

        std::string get_type_name() const override {
            return fmt::format("mt::stdlib::limiter_block_const<{}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT));
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

        std::vector<std::string> constructor_arguments() const override {
            return {
                fmt::format("{}", _upper),
                fmt::format("{}", _lower),
            };
        }

        const limit_t _upper;
        const limit_t _lower;
    };

    class LimiterExecutor : public BlockExecutionInterface {
    public:
        LimiterExecutor(std::shared_ptr<const ModelValue> ptr_input, std::shared_ptr<const ModelValue> val_min,
                        std::shared_ptr<const ModelValue> val_max, std::shared_ptr<ModelValue> ptr_output)
            : _ptr_input(std::dynamic_pointer_cast<const ModelValueBox<DT>>(ptr_input)),
              _ptr_output(std::dynamic_pointer_cast<ModelValueBox<DT>>(ptr_output)),
              _val_min(std::dynamic_pointer_cast<const ModelValueBox<DT>>(val_min)),
              _val_max(std::dynamic_pointer_cast<const ModelValueBox<DT>>(val_max)) {
            if (_ptr_input == nullptr || _ptr_output == nullptr || _val_min == nullptr || _val_max == nullptr) {
                throw ModelException("input pointers cannot be null");
            }
        }

    protected:
        void update_inputs() override {
            block.s_in.value = _ptr_input->value;
            block.s_in.limit_lower = _val_min->value;
            block.s_in.limit_upper = _val_max->value;
        }

        void update_outputs() override { _ptr_output->value = block.s_out.value; }

        void blk_reset() override { block.reset(); }

        void blk_step() override { block.step(); }

    private:
        std::shared_ptr<const ModelValueBox<DT>> _ptr_input;
        std::shared_ptr<ModelValueBox<DT>> _ptr_output;
        std::shared_ptr<const ModelValueBox<DT>> _val_min;
        std::shared_ptr<const ModelValueBox<DT>> _val_max;

        mt::stdlib::limiter_block<tmdl::data_type_t<DT>::mt_data_type> block;
    };
};

Limiter::Limiter() {
    // Setup parameters
    using enum DataType;

    dynamicLimiter = std::make_shared<Parameter>("dynamic_limiter", "Use Dynamic Limits", std::make_unique<ModelValueBox<BOOLEAN>>(false));

    prmMaxValue = std::make_shared<Parameter>("max_value", "Maximum Value", std::make_unique<ModelValueBox<UNKNOWN>>());

    prmMinValue = std::make_shared<Parameter>("min_value", "Minimum Value", std::make_unique<ModelValueBox<UNKNOWN>>());

    input_type = UNKNOWN;
    output_port = UNKNOWN;
    input_type_min = UNKNOWN;
    input_type_max = UNKNOWN;
}

std::string Limiter::get_name() const { return "limiter"; }

std::string Limiter::get_description() const { return "Limits input values by the provided parameters"; }

std::vector<std::shared_ptr<Parameter>> Limiter::get_parameters() const { return {dynamicLimiter, prmMinValue, prmMaxValue}; }

size_t Limiter::get_num_inputs() const {
    if (use_dynamic_limit()) {
        return 3;
    } else {
        return 1;
    }
}

size_t Limiter::get_num_outputs() const { return 1; }

bool Limiter::update_block() {
    bool updated = false;
    const bool use_dyn_lim = use_dynamic_limit();

    prmMaxValue->set_enabled(!use_dyn_lim);
    prmMinValue->set_enabled(!use_dyn_lim);

    if (input_type != output_port) {
        DataType new_dtype = DataType::UNKNOWN;

        switch (input_type) {
            using enum tmdl::DataType;
        case DOUBLE:
        case SINGLE:
        case UINT32:
        case INT32:
            new_dtype = input_type;
            break;
        default:
            break;
        }

        if (new_dtype != DataType::UNKNOWN) {
            prmMaxValue->convert_type(new_dtype);
            prmMinValue->convert_type(new_dtype);
        }

        output_port = input_type;
        updated = true;
    }

    return updated;
}

std::unique_ptr<const BlockError> Limiter::has_error() const {
    const std::vector<DataType> supportedDataTypes = {
        DataType::DOUBLE,
        DataType::SINGLE,
        DataType::INT32,
        DataType::UINT32,
    };

    if (std::ranges::find(supportedDataTypes, input_type) == supportedDataTypes.end()) {
        return make_error("input port not set");
    }

    if (use_dynamic_limit()) {
        if (input_type_max != input_type || input_type_min != input_type) {
            return make_error("min/max data types should be the same data type as the primary input");
        }
    } else {
        for (const auto& p : get_parameters()) {
            if (p->get_value()->data_type() == DataType::UNKNOWN) {
                return make_error("min or max value is not able to be set");
            }
        }
    }

    return nullptr;
}

void Limiter::set_input_type(const size_t port, const DataType type) {
    if (port < get_num_inputs()) {
        switch (port) {
        case 0:
            input_type = type;
            break;
        case 1:
            input_type_min = type;
            break;
        case 2:
            input_type_max = type;
            break;
        default:
            throw ModelException("unexpected input port provided");
        }
    } else {
        throw ModelException("provided input port too high");
    }
}

DataType Limiter::get_output_type(const size_t port) const {
    if (port < get_num_outputs()) {
        return output_port;
    } else {
        throw ModelException("provided output port is too high");
    }
}

std::unique_ptr<CompiledBlockInterface> Limiter::get_compiled(const ModelInfo&) const {
    if (has_error() != nullptr) {
        throw ModelException("cannot execute limiter with incomplete input parameters");
    }

    std::optional<LimiterValues> limitValues{};

    if (!use_dynamic_limit()) {
        limitValues = LimiterValues{.upper = prmMaxValue->get_value()->clone(), .lower = prmMinValue->get_value()->clone()};
    }

    switch (input_type) {
        using enum tmdl::DataType;
    case DOUBLE:
        return std::make_unique<CompiledLimiter<DOUBLE>>(get_id(), std::move(limitValues));
    case SINGLE:
        return std::make_unique<CompiledLimiter<SINGLE>>(get_id(), std::move(limitValues));
    case INT32:
        return std::make_unique<CompiledLimiter<INT32>>(get_id(), std::move(limitValues));
    case UINT32:
        return std::make_unique<CompiledLimiter<UINT32>>(get_id(), std::move(limitValues));
    default:
        throw ModelException("unable to generate limitor executor");
    }
}

bool Limiter::use_dynamic_limit() const { return ModelValue::get_inner_value<DataType::BOOLEAN>(dynamicLimiter->get_value()); }
