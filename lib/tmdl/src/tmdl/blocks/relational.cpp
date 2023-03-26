// SPDX-License-IdentifierDiDivision3.0-only

#include "relational.hpp"

#include "../model_exception.hpp"

#include <memory>
#include <algorithm>

#include <tmdlstd/relational.hpp>

// Relational Executor

template <tmdl::DataType DT, tmdlstd::RelationalOperator OP>
struct RelationalExecutor : public tmdl::BlockExecutionInterface
{
    using type_t = tmdl::data_type_t<DT>::type;

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

    tmdlstd::relational_block<type_t, OP> block;
};

// Relational Base

tmdl::stdlib::RelationalBase::RelationalBase()
{
    _inputA = DataType::UNKNOWN;
    _inputB = DataType::UNKNOWN;
    _outputPort = DataType::BOOLEAN;
}

size_t tmdl::stdlib::RelationalBase::get_num_inputs() const
{
    return 2;
}

size_t tmdl::stdlib::RelationalBase::get_num_outputs() const
{
    return 1;
}

bool tmdl::stdlib::RelationalBase::update_block()
{
    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::stdlib::RelationalBase::has_error() const
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

    /*
    if (fcns.double_fcn) availableTypes.push_back(DataType::DOUBLE);
    if (fcns.float_fcn) availableTypes.push_back(DataType::SINGLE);
    if (fcns.i32_fcn) availableTypes.push_back(DataType::INT32);
    if (fcns.u32_fcn) availableTypes.push_back(DataType::UINT32);
    if (fcns.bool_fcn) availableTypes.push_back(DataType::BOOLEAN);
    */ // TODO - Add support for specific types

    if (std::find(availableTypes.begin(), availableTypes.end(), _inputA) == availableTypes.end())
    {
        return make_error("relational block does not support provided types");
    }

    return nullptr;
}

void tmdl::stdlib::RelationalBase::set_input_type(
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

tmdl::DataType tmdl::stdlib::RelationalBase::get_output_type(const size_t port) const
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

template<tmdlstd::RelationalOperator OP>
static std::shared_ptr<tmdl::BlockExecutionInterface> generate_execution_interface(
    const tmdl::stdlib::RelationalBase* model,
    const tmdl::ConnectionManager& connections,
    const tmdl::VariableManager& manager)
{
    if (model->has_error() != nullptr)
    {
        throw tmdl::ModelException("cannot generate a model with an error");
    }

    std::shared_ptr<const tmdl::ModelValue> input_a = manager.get_ptr(*connections.get_connection_to(model->get_id(), 0));
    std::shared_ptr<const tmdl::ModelValue> input_b = manager.get_ptr(*connections.get_connection_to(model->get_id(), 1));

    auto output_value = std::dynamic_pointer_cast<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>>(manager.get_ptr(tmdl::VariableIdentifier
    {
        .block_id = model->get_id(),
        .output_port_num = 0
    }));

    switch (input_a->get_data_type())
    {
    case tmdl::DataType::DOUBLE:
        return std::make_shared<RelationalExecutor<tmdl::DataType::DOUBLE, OP>>(
            input_a,
            input_b,
            output_value);
    case tmdl::DataType::SINGLE:
        return std::make_shared<RelationalExecutor<tmdl::DataType::SINGLE, OP>>(
            input_a,
            input_b,
            output_value);
    case tmdl::DataType::INT32:
        return std::make_shared<RelationalExecutor<tmdl::DataType::INT32, OP>>(
            input_a,
            input_b,
            output_value);
    case tmdl::DataType::UINT32:
        return std::make_shared<RelationalExecutor<tmdl::DataType::UINT32, OP>>(
            input_a,
            input_b,
            output_value);
    case tmdl::DataType::BOOLEAN:
        return std::make_shared<RelationalExecutor<tmdl::DataType::BOOLEAN, OP>>(
            input_a,
            input_b,
            output_value);
    default:
        throw tmdl::ModelException("unable to generate relational executor");
    }
}

// GreaterThan Block

std::string tmdl::stdlib::GreaterThan::get_name() const
{
    return ">";
}

std::string tmdl::stdlib::GreaterThan::get_description() const
{
    return get_name();
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::GreaterThan::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    return generate_execution_interface<tmdlstd::RelationalOperator::GREATER_THAN>(this, connections, manager);
}

// GreaterThanEqual Block

std::string tmdl::stdlib::GreaterThanEqual::get_name() const
{
    return ">=";
}

std::string tmdl::stdlib::GreaterThanEqual::get_description() const
{
    return get_name();
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::GreaterThanEqual::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    return generate_execution_interface<tmdlstd::RelationalOperator::GREATER_THAN_EQUAL>(this, connections, manager);
}

// LessThan Block

std::string tmdl::stdlib::LessThan::get_name() const
{
    return "<";
}

std::string tmdl::stdlib::LessThan::get_description() const
{
    return get_name();
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::LessThan::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    return generate_execution_interface<tmdlstd::RelationalOperator::LESS_THAN>(this, connections, manager);
}

// LessThanEqual Block

std::string tmdl::stdlib::LessThanEqual::get_name() const
{
    return "<=";
}

std::string tmdl::stdlib::LessThanEqual::get_description() const
{
    return get_name();
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::LessThanEqual::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    return generate_execution_interface<tmdlstd::RelationalOperator::LESS_THAN_EQUAL>(this, connections, manager);
}

// Equal Block

std::string tmdl::stdlib::Equal::get_name() const
{
    return "==";
}

std::string tmdl::stdlib::Equal::get_description() const
{
    return get_name();
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Equal::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    return generate_execution_interface<tmdlstd::RelationalOperator::EQUAL>(this, connections, manager);
}

// NotEqual Block

std::string tmdl::stdlib::NotEqual::get_name() const
{
    return "!=";
}

std::string tmdl::stdlib::NotEqual::get_description() const
{
    return get_name();
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::NotEqual::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    return generate_execution_interface<tmdlstd::RelationalOperator::NOT_EQUAL>(this, connections, manager);
}
