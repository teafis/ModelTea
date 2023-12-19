// SPDX-License-Identifier: GPL-3.0-only

#include "constant.hpp"

#include "../model_exception.hpp"
#include "tmdl/values/value.hpp"

#include <fmt/format.h>
#include <tmdlstd/tmdlstd.hpp>

tmdl::blocks::Constant::Constant()
    : param_dtype(std::make_shared<Parameter>("dtype", "data type", ModelValue::make_default(DataType::DATA_TYPE))),
      param_value(std::make_shared<Parameter>("value", "Constant Value", ModelValue::make_default(DataType::UNKNOWN))) {
    // Empty Constructor
}

std::string tmdl::blocks::Constant::get_name() const { return "constant"; }

std::string tmdl::blocks::Constant::get_description() const { return "Provides a constant value output"; }

size_t tmdl::blocks::Constant::get_num_inputs() const { return 0; }

size_t tmdl::blocks::Constant::get_num_outputs() const { return 1; }

bool tmdl::blocks::Constant::update_block() {
    if (const auto selected_dtype = get_data_type(); selected_dtype != output_port) {
        output_port = selected_dtype;

        std::ostringstream oss;
        oss << "Constant Value (" << data_type_to_string(output_port) << ")";

        param_value->set_name(oss.str());
        param_value->convert_type(output_port);

        return true;
    }

    return false;
}

std::vector<std::shared_ptr<tmdl::Parameter>> tmdl::blocks::Constant::get_parameters() const { return {param_dtype, param_value}; }

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::Constant::has_error() const {
    if (const auto current_dtype = get_data_type();
        current_dtype == DataType::UNKNOWN || current_dtype != param_value->get_value()->data_type()) {
        return make_error("data type provided is of unknown type");
    }

    return nullptr;
}

void tmdl::blocks::Constant::set_input_type(const size_t, const DataType) { throw ModelException("input port out of range"); }

tmdl::DataType tmdl::blocks::Constant::get_output_type(const size_t port) const {
    if (port == 0) {
        return output_port;
    } else {
        throw ModelException("output port out of range");
    }
}

template <tmdl::DataType DT> class CompiledConstant : public tmdl::CompiledBlockInterface {
    static_assert(tmdl::data_type_t<DT>::is_modelable);

public:
    using type_t = typename tmdl::data_type_t<DT>::type;

    explicit CompiledConstant(const size_t id, const type_t value) : _id{id}, _value{value} {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(const tmdl::ConnectionManager&,
                                                                           const tmdl::VariableManager& manager) const override {
        const auto output_type = manager.get_ptr(tmdl::VariableIdentifier{.block_id = _id, .output_port_num = 0});

        return std::make_shared<ConstantExecutor>(_value, output_type);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override { return std::make_unique<ConstantComponent>(_value); }

private:
    const size_t _id;
    const type_t _value;

protected:
    struct ConstantComponent : public tmdl::codegen::CodeComponent {
        explicit ConstantComponent(type_t val) : value(val) {
            // Empty Constructor
        }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override { return {}; }

        std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override {
            return tmdl::codegen::InterfaceDefinition("s_out", {"val"});
        }

        std::string get_module_name() const override { return "tmdlstd/tmdlstd.hpp"; }

        std::string get_name_base() const override { return "const_block"; }

        std::string get_type_name() const override {
            return fmt::format("tmdl::stdlib::const_block<{}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT));
        }

        std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction) const override { return {}; }

        std::vector<std::string> constructor_arguments() const override { return {std::to_string(value)}; }

        const type_t value;
    };

    struct ConstantExecutor : public tmdl::BlockExecutionInterface {
        explicit ConstantExecutor(const type_t& value, const std::shared_ptr<tmdl::ModelValue> ptr) {
            const_ptr = std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(ptr);
            if (const_ptr == nullptr) {
                throw tmdl::ModelException("provided output pointer cannot be null");
            }

            block = std::make_unique<tmdl::stdlib::const_block<type_t>>(value);
            update_outputs();
        }

        std::shared_ptr<tmdl::ModelValueBox<DT>> const_ptr;
        std::unique_ptr<tmdl::stdlib::const_block<type_t>> block;

    protected:
        void update_inputs() override {}
        void update_outputs() override { const_ptr->value = block->s_out.value; }
    };
};

template <tmdl::DataType DT>
static std::unique_ptr<tmdl::CompiledBlockInterface> create_compiled(const size_t id, const tmdl::ModelValue* value) {
    return std::make_unique<CompiledConstant<DT>>(id, tmdl::ModelValue::get_inner_value<DT>(value));
}

std::unique_ptr<tmdl::CompiledBlockInterface> tmdl::blocks::Constant::get_compiled(const ModelInfo&) const {
    if (has_error() != nullptr) {
        throw ModelException("cannot build constant executor with error present");
    }

    switch (param_value->get_value()->data_type()) {
        using enum DataType;
    case DOUBLE:
        return create_compiled<DOUBLE>(get_id(), param_value->get_value());
    case SINGLE:
        return create_compiled<SINGLE>(get_id(), param_value->get_value());
    case BOOLEAN:
        return create_compiled<BOOLEAN>(get_id(), param_value->get_value());
    case INT32:
        return create_compiled<INT32>(get_id(), param_value->get_value());
    case UINT32:
        return create_compiled<UINT32>(get_id(), param_value->get_value());
    default:
        throw ModelException("unknown data type provided for executor");
    }
}

tmdl::DataType tmdl::blocks::Constant::get_data_type() const {
    return ModelValue::get_inner_value<DataType::DATA_TYPE>(param_dtype->get_value());
}
