// SPDX-License-IdentifierDiDivision3.0-only

#include "relational.hpp"

#include "../model_exception.hpp"

#include <memory>
#include <algorithm>

#include <fmt/format.h>

#include <tmdlstd/relational.hpp>
#include <tmdlstd/util.hpp>

// Relational Executor

template <tmdl::DataType DT, tmdl::stdlib::RelationalOperator OP>
struct RelationalHelper : public tmdl::CodegenHelperInterface::HelperInterface
{
    struct RelationalComponent : public tmdl::codegen::CodeComponent
    {
        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_in", {"val_a", "val_b"});
        }

        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_out", {"output_value"});
        }

        virtual std::string get_include_file_name() const override
        {
            return "tmdlstd/relational.hpp";
        }

        virtual std::string get_name_base() const override
        {
            return "relational_block";
        }

        virtual std::string get_type_name() const override
        {
            return fmt::format("tmdlstd::relational_block<{}, {}>", tmdl::data_type_to_string(DT), tmdl::stdlib::relational_to_string(OP));
        }

        virtual std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override
        {
            switch (ft)
            {
            case tmdl::codegen::BlockFunction::STEP:
                return "step";
            default:
                return {};
            }
        }
    };

    struct RelationalExecutor : public tmdl::BlockExecutionInterface
    {
        using type_t = typename tmdl::data_type_t<DT>::type;

        RelationalExecutor(
            std::shared_ptr<const tmdl::ModelValue> inputA,
            std::shared_ptr<const tmdl::ModelValue> inputB,
            std::shared_ptr<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> outputValue) :
            output_value(outputValue)
        {
            input_a = std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(inputA);
            input_b = std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(inputB);

            if (output_value == nullptr || input_a == nullptr || input_b == nullptr)
            {
                throw tmdl::ModelException("provided pointers must be non-null");
            }

            block.s_in.val_a = &input_a->value;
            block.s_in.val_b = &input_b->value;
        }

        void step(const tmdl::SimState&) override
        {
            block.step();
            output_value->value = block.s_out.output_value;
        }

    protected:
        std::shared_ptr<const tmdl::ModelValueBox<DT>> input_a;
        std::shared_ptr<const tmdl::ModelValueBox<DT>> input_b;
        std::shared_ptr<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> output_value;

        tmdl::stdlib::relational_block<type_t, OP> block;
    };

    std::shared_ptr<tmdl::BlockExecutionInterface> generate_execution_interface(
        const tmdl::BlockInterface* model,
        const tmdl::ConnectionManager& connections,
        const tmdl::VariableManager& manager) const
    {
        std::shared_ptr<const tmdl::ModelValue> input_a = manager.get_ptr(*connections.get_connection_to(model->get_id(), 0));
        std::shared_ptr<const tmdl::ModelValue> input_b = manager.get_ptr(*connections.get_connection_to(model->get_id(), 1));

        auto output_value = std::dynamic_pointer_cast<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>>(manager.get_ptr(tmdl::VariableIdentifier
        {
            .block_id = model->get_id(),
            .output_port_num = 0
        }));

        return std::make_shared<RelationalExecutor>(
            input_a,
            input_b,
            output_value);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> generate_codegen_interface() const
    {
        return std::make_unique<RelationalComponent>();
    }
};

// Relational Base

tmdl::blocks::RelationalBase::RelationalBase()
{
    _inputA = DataType::UNKNOWN;
    _inputB = DataType::UNKNOWN;
    _outputPort = DataType::BOOLEAN;
}

size_t tmdl::blocks::RelationalBase::get_num_inputs() const
{
    return 2;
}

size_t tmdl::blocks::RelationalBase::get_num_outputs() const
{
    return 1;
}

bool tmdl::blocks::RelationalBase::update_block()
{
    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::RelationalBase::has_error() const
{
    if (_inputA != _inputB)
    {
        return make_error("input port must have the same type");
    }

    std::vector<DataType> availableTypes{
        DataType::DOUBLE,
        DataType::SINGLE,
        DataType::INT32,
        DataType::UINT32,
        DataType::BOOLEAN,
    };

    if (std::find(availableTypes.begin(), availableTypes.end(), _inputA) == availableTypes.end())
    {
        return make_error("relational block does not support provided types");
    }

    return nullptr;
}

void tmdl::blocks::RelationalBase::set_input_type(
    const size_t port,
    const DataType type)
{
    switch (port)
    {
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

tmdl::DataType tmdl::blocks::RelationalBase::get_output_type(const size_t port) const
{
    if (port == 0)
    {
        return _outputPort;
    }
    else
    {
        throw ModelException("output port is out of range");
    }
}

std::vector<tmdl::DataType> tmdl::blocks::RelationalNumericBase::get_supported_types() const
{
    return {
        tmdl::DataType::DOUBLE,
        tmdl::DataType::SINGLE,
        tmdl::DataType::INT32,
        tmdl::DataType::UINT32,
    };
}

std::vector<tmdl::DataType> tmdl::blocks::RelationalEqualityBase::get_supported_types() const
{
    return {
        tmdl::DataType::INT32,
        tmdl::DataType::UINT32,
        tmdl::DataType::BOOLEAN,
    };
}

// Helper Generator

template <tmdl::stdlib::RelationalOperator OP>
static std::unique_ptr<tmdl::CodegenHelperInterface::HelperInterface> generate_helper(const tmdl::blocks::RelationalBase* model)
{
    if (model->has_error() != nullptr)
    {
        throw tmdl::ModelException("cannot generate a model with an error");
    }

    const auto dts = model->get_supported_types();
    const auto dt = model->get_output_type(0);

    if (std::find(dts.begin(), dts.end(), dt) == dts.end())
    {
        throw tmdl::ModelException("data type is unsupported for block");
    }

    switch (dt)
    {
    case tmdl::DataType::DOUBLE:
        return std::make_unique<RelationalHelper<tmdl::DataType::DOUBLE, OP>>();
    case tmdl::DataType::SINGLE:
        return std::make_unique<RelationalHelper<tmdl::DataType::SINGLE, OP>>();
    case tmdl::DataType::INT32:
        return std::make_unique<RelationalHelper<tmdl::DataType::INT32, OP>>();
    case tmdl::DataType::UINT32:
        return std::make_unique<RelationalHelper<tmdl::DataType::UINT32, OP>>();
    case tmdl::DataType::BOOLEAN:
        return std::make_unique<RelationalHelper<tmdl::DataType::BOOLEAN, OP>>();
    default:
        throw tmdl::ModelException("unable to generate relational instance");
    }
}

// GreaterThan Block

std::string tmdl::blocks::GreaterThan::get_name() const
{
    return ">";
}

std::string tmdl::blocks::GreaterThan::get_description() const
{
    return get_name();
}

std::unique_ptr<tmdl::CodegenHelperInterface::HelperInterface> tmdl::blocks::GreaterThan::get_helper_interface() const
{
    return generate_helper<tmdl::stdlib::RelationalOperator::GREATER_THAN>(this);
}

// GreaterThanEqual Block

std::string tmdl::blocks::GreaterThanEqual::get_name() const
{
    return ">=";
}

std::string tmdl::blocks::GreaterThanEqual::get_description() const
{
    return get_name();
}

std::unique_ptr<tmdl::CodegenHelperInterface::HelperInterface> tmdl::blocks::GreaterThanEqual::get_helper_interface() const
{
    return generate_helper<tmdl::stdlib::RelationalOperator::GREATER_THAN_EQUAL>(this);
}

// LessThan Block

std::string tmdl::blocks::LessThan::get_name() const
{
    return "<";
}

std::string tmdl::blocks::LessThan::get_description() const
{
    return get_name();
}

std::unique_ptr<tmdl::CodegenHelperInterface::HelperInterface> tmdl::blocks::LessThan::get_helper_interface() const
{
    return generate_helper<tmdl::stdlib::RelationalOperator::LESS_THAN>(this);
}

// LessThanEqual Block

std::string tmdl::blocks::LessThanEqual::get_name() const
{
    return "<=";
}

std::string tmdl::blocks::LessThanEqual::get_description() const
{
    return get_name();
}

std::unique_ptr<tmdl::CodegenHelperInterface::HelperInterface> tmdl::blocks::LessThanEqual::get_helper_interface() const
{
    return generate_helper<tmdl::stdlib::RelationalOperator::LESS_THAN_EQUAL>(this);
}

// Equal Block

std::string tmdl::blocks::Equal::get_name() const
{
    return "==";
}

std::string tmdl::blocks::Equal::get_description() const
{
    return get_name();
}

std::unique_ptr<tmdl::CodegenHelperInterface::HelperInterface> tmdl::blocks::Equal::get_helper_interface() const
{
    return generate_helper<tmdl::stdlib::RelationalOperator::EQUAL>(this);
}

// NotEqual Block

std::string tmdl::blocks::NotEqual::get_name() const
{
    return "!=";
}

std::string tmdl::blocks::NotEqual::get_description() const
{
    return get_name();
}

std::unique_ptr<tmdl::CodegenHelperInterface::HelperInterface> tmdl::blocks::NotEqual::get_helper_interface() const
{
    return generate_helper<tmdl::stdlib::RelationalOperator::NOT_EQUAL>(this);
}
