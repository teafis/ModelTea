// SPDX-License-Identifier: GPL-3.0-only

#include "relational.hpp"

#include "../model_exception.hpp"

#include <algorithm>
#include <memory>

#include <fmt/format.h>

#include <mtstd.hpp>
#include <mtstd_string.hpp>

// Relational Executor

template <tmdl::DataType DT, mt::stdlib::RelationalOperator OP> class CompiledRelational : public tmdl::CompiledBlockInterface {
public:
    explicit CompiledRelational(const size_t id) : _id(id) {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(const tmdl::ConnectionManager& connections,
                                                                           const tmdl::VariableManager& manager) const override {
        std::shared_ptr<const tmdl::ModelValue> input_a = manager.get_ptr(*connections.get_connection_to(_id, 0));
        std::shared_ptr<const tmdl::ModelValue> input_b = manager.get_ptr(*connections.get_connection_to(_id, 1));

        auto output_value = std::dynamic_pointer_cast<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>>(
            manager.get_ptr(tmdl::VariableIdentifier{.block_id = _id, .output_port_num = 0}));

        return std::make_shared<RelationalExecutor>(input_a, input_b, output_value);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override { return std::make_unique<RelationalComponent>(); }

private:
    const size_t _id;

protected:
    struct RelationalComponent : public tmdl::codegen::CodeComponent {
        std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_in", {"value_a", "value_b"});
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_out", {"value"});
        }

        std::string get_module_name() const override { return "tmdlstd/tmdlstd.hpp"; }

        std::string get_name_base() const override { return "relational_block"; }

        std::string get_type_name() const override {
            return fmt::format("mt::stdlib::relational_block<{}, {}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT),
                               mt::stdlib::relational_to_string(OP));
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
    };

    struct RelationalExecutor : public tmdl::BlockExecutionInterface {
        using type_t = typename tmdl::data_type_t<DT>::type;

        RelationalExecutor(std::shared_ptr<const tmdl::ModelValue> inputA, std::shared_ptr<const tmdl::ModelValue> inputB,
                           std::shared_ptr<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> outputValue)
            : output_value(outputValue) {
            input_a = std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(inputA);
            input_b = std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(inputB);

            if (output_value == nullptr || input_a == nullptr || input_b == nullptr) {
                throw tmdl::ModelException("provided pointers must be non-null");
            }
        }

    protected:
        void update_inputs() override {
            block.s_in.value_a = input_a->value;
            block.s_in.value_b = input_b->value;
        }

        void update_outputs() override { output_value->value = block.s_out.value; }

        void blk_reset() override { block.reset(); }

        void blk_step() override { block.step(); }

    private:
        std::shared_ptr<const tmdl::ModelValueBox<DT>> input_a;
        std::shared_ptr<const tmdl::ModelValueBox<DT>> input_b;
        std::shared_ptr<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> output_value;

        mt::stdlib::relational_block<tmdl::data_type_t<DT>::mt_data_type, OP> block;
    };
};

// Relational Base

tmdl::blocks::RelationalBase::RelationalBase() {
    using enum DataType;
    _inputA = UNKNOWN;
    _inputB = UNKNOWN;
    _outputPort = BOOLEAN;
}

size_t tmdl::blocks::RelationalBase::get_num_inputs() const { return 2; }

size_t tmdl::blocks::RelationalBase::get_num_outputs() const { return 1; }

bool tmdl::blocks::RelationalBase::update_block() { return false; }

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::RelationalBase::has_error() const {
    if (_inputA != _inputB) {
        return make_error("input port must have the same type");
    }

    if (const std::vector<DataType> aTypes = get_supported_types(); std::ranges::find(aTypes, _inputA) == aTypes.end()) {
        return make_error(fmt::format("relational block {} does not support provided types", get_name()));
    }

    return nullptr;
}

void tmdl::blocks::RelationalBase::set_input_type(const size_t port, const DataType type) {
    switch (port) {
    case 0:
        _inputA = type;
        break;
    case 1:
        _inputB = type;
        break;
    default:
        throw ModelException("provided port value exceeds input port count");
    }
}

tmdl::DataType tmdl::blocks::RelationalBase::get_output_type(const size_t port) const {
    if (port == 0) {
        return _outputPort;
    } else {
        throw ModelException("output port is out of range");
    }
}

std::vector<tmdl::DataType> tmdl::blocks::RelationalNumericBase::get_supported_types() const {
    using enum DataType;
    return {
        DOUBLE,
        SINGLE,
        INT32,
        UINT32,
    };
}

tmdl::DataType tmdl::blocks::RelationalBase::get_input_type() const {
    if (_inputA != _inputB) {
        throw ModelException("mismatch in input types");
    }

    return _inputA;
}

std::vector<tmdl::DataType> tmdl::blocks::RelationalEqualityBase::get_supported_types() const {
    using enum DataType;
    return {
        INT32,
        UINT32,
        BOOLEAN,
    };
}

// Helper Generator

template <mt::stdlib::RelationalOperator OP>
static std::unique_ptr<tmdl::CompiledBlockInterface> generate_compiled(const tmdl::blocks::RelationalBase* model, tmdl::DataType dataType) {
    if (model->has_error() != nullptr) {
        throw tmdl::ModelException(fmt::format("cannot generate a {} model with an error", model->get_name()));
    }

    const auto dt = dataType;
    const auto id = model->get_id();

    switch (dt) {
        using enum tmdl::DataType;
    case DOUBLE:
        return std::make_unique<CompiledRelational<DOUBLE, OP>>(id);
    case SINGLE:
        return std::make_unique<CompiledRelational<SINGLE, OP>>(id);
    case INT32:
        return std::make_unique<CompiledRelational<INT32, OP>>(id);
    case UINT32:
        return std::make_unique<CompiledRelational<UINT32, OP>>(id);
    case BOOLEAN:
        return std::make_unique<CompiledRelational<BOOLEAN, OP>>(id);
    default:
        throw tmdl::ModelException("unable to generate relational instance");
    }
}

// GreaterThan Block

std::string tmdl::blocks::GreaterThan::get_name() const { return ">"; }

std::string tmdl::blocks::GreaterThan::get_description() const { return get_name(); }

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::GreaterThan::get_compiled(const ModelInfo&) const {
    return generate_compiled<mt::stdlib::RelationalOperator::GREATER_THAN>(this, get_input_type());
}

// GreaterThanEqual Block

std::string tmdl::blocks::GreaterThanEqual::get_name() const { return ">="; }

std::string tmdl::blocks::GreaterThanEqual::get_description() const { return get_name(); }

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::GreaterThanEqual::get_compiled(const ModelInfo&) const {
    return generate_compiled<mt::stdlib::RelationalOperator::GREATER_THAN_EQUAL>(this, get_input_type());
}

// LessThan Block

std::string tmdl::blocks::LessThan::get_name() const { return "<"; }

std::string tmdl::blocks::LessThan::get_description() const { return get_name(); }

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::LessThan::get_compiled(const ModelInfo&) const {
    return generate_compiled<mt::stdlib::RelationalOperator::LESS_THAN>(this, get_input_type());
}

// LessThanEqual Block

std::string tmdl::blocks::LessThanEqual::get_name() const { return "<="; }

std::string tmdl::blocks::LessThanEqual::get_description() const { return get_name(); }

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::LessThanEqual::get_compiled(const ModelInfo&) const {
    return generate_compiled<mt::stdlib::RelationalOperator::LESS_THAN_EQUAL>(this, get_input_type());
}

// Equal Block

std::string tmdl::blocks::Equal::get_name() const { return "=="; }

std::string tmdl::blocks::Equal::get_description() const { return get_name(); }

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::Equal::get_compiled(const ModelInfo&) const {
    return generate_compiled<mt::stdlib::RelationalOperator::EQUAL>(this, get_input_type());
}

// NotEqual Block

std::string tmdl::blocks::NotEqual::get_name() const { return "!="; }

std::string tmdl::blocks::NotEqual::get_description() const { return get_name(); }

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::NotEqual::get_compiled(const ModelInfo&) const {
    return generate_compiled<mt::stdlib::RelationalOperator::NOT_EQUAL>(this, get_input_type());
}
