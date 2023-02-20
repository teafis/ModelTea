// SPDX-License-IdentifierDiDivision3.0-only

#include "relational.hpp"

#include "../model_exception.hpp"

#include <memory>
#include <algorithm>

// Arithmetic Executor

template <typename T>
bool tsim_eq(const T& a, const T& b)
{
    return a == b;
}

template <typename T>
bool tsim_neq(const T& a, const T& b)
{
    return a != b;
}

template <typename T>
bool tsim_gt(const T& a, const T& b)
{
    return a > b;
}

template <typename T>
bool tsim_geq(const T& a, const T& b)
{
    return a >= b;
}

template <typename T>
bool tsim_lt(const T& a, const T& b)
{
    return a < b;
}

template <typename T>
bool tsim_leq(const T& a, const T& b)
{
    return a <= b;
}

template <typename T>
using rel_op_fn_t = bool (*)(const T&, const T&);

//template <template <typename> class FCN, typename T>
template <tmdl::DataType DT>
struct RelationalExecutor : public tmdl::BlockExecutionInterface
{
    using input_type_t = tmdl::data_type_t<DT>::type;

    RelationalExecutor(
        std::shared_ptr<const tmdl::ModelValue> inputA,
        std::shared_ptr<const tmdl::ModelValue> inputB,
        std::shared_ptr<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> outputValue,
        rel_op_fn_t<input_type_t> func) :
        output_value(outputValue),
        func(func)
    {
        input_a = std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(inputA);
        input_b = std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(inputB);

        if (output_value == nullptr || input_a == nullptr || input_b == nullptr)
        {
            throw tmdl::ModelException("provided pointers must be non-null");
        }
        else if (func == nullptr)
        {
            throw tmdl::ModelException("provided function cannot be null");
        }
    }

    void step(const tmdl::SimState&) override
    {
        output_value->value = func(input_a->value, input_b->value);
    }

protected:
    std::shared_ptr<const tmdl::ModelValueBox<DT>> input_a;
    std::shared_ptr<const tmdl::ModelValueBox<DT>> input_b;
    std::shared_ptr<tmdl::ModelValueBox<tmdl::DataType::BOOLEAN>> output_value;
    rel_op_fn_t<input_type_t> func;
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

    const auto fcns = get_application_functions();
    std::vector<DataType> availableTypes;

    if (fcns.double_fcn) availableTypes.push_back(DataType::DOUBLE);
    if (fcns.float_fcn) availableTypes.push_back(DataType::SINGLE);
    if (fcns.i32_fcn) availableTypes.push_back(DataType::INT32);
    if (fcns.u32_fcn) availableTypes.push_back(DataType::UINT32);
    if (fcns.bool_fcn) availableTypes.push_back(DataType::BOOLEAN);

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

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::RelationalBase::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot generate a model with an error");
    }

    std::shared_ptr<const ModelValue> input_a = manager.get_ptr(*connections.get_connection_to(get_id(), 0));
    std::shared_ptr<const ModelValue> input_b = manager.get_ptr(*connections.get_connection_to(get_id(), 1));

    auto output_value = std::dynamic_pointer_cast<ModelValueBox<DataType::BOOLEAN>>(manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    }));

    const FunctionTypes fcs = get_application_functions();

    switch (_inputA)
    {
    case DataType::DOUBLE:
        return std::make_shared<RelationalExecutor<DataType::DOUBLE>>(
            input_a,
            input_b,
            output_value,
            fcs.double_fcn);
    case DataType::SINGLE:
        return std::make_shared<RelationalExecutor<DataType::SINGLE>>(
            input_a,
            input_b,
            output_value,
            fcs.float_fcn);
    case DataType::INT32:
        return std::make_shared<RelationalExecutor<DataType::INT32>>(
            input_a,
            input_b,
            output_value,
            fcs.i32_fcn);
    case DataType::UINT32:
        return std::make_shared<RelationalExecutor<DataType::UINT32>>(
            input_a,
            input_b,
            output_value,
            fcs.u32_fcn);
    case DataType::BOOLEAN:
        return std::make_shared<RelationalExecutor<DataType::BOOLEAN>>(
            input_a,
            input_b,
            output_value,
            fcs.bool_fcn);
    default:
        throw ModelException("unable to generate arithmetic executor");
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

tmdl::stdlib::RelationalBase::FunctionTypes tmdl::stdlib::GreaterThan::get_application_functions() const
{
    return FunctionTypes
    {
        .double_fcn = tsim_gt<double>,
        .float_fcn = tsim_gt<float>,
        .i32_fcn = tsim_gt<int32_t>,
        .u32_fcn = tsim_gt<uint32_t>,
        .bool_fcn = nullptr
    };
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

tmdl::stdlib::RelationalBase::FunctionTypes tmdl::stdlib::GreaterThanEqual::get_application_functions() const
{
    return FunctionTypes
    {
        .double_fcn = tsim_geq<double>,
        .float_fcn = tsim_geq<float>,
        .i32_fcn = tsim_geq<int32_t>,
        .u32_fcn = tsim_geq<uint32_t>,
        .bool_fcn = nullptr
    };
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

tmdl::stdlib::RelationalBase::FunctionTypes tmdl::stdlib::LessThan::get_application_functions() const
{
    return FunctionTypes
    {
        .double_fcn = tsim_lt<double>,
        .float_fcn = tsim_lt<float>,
        .i32_fcn = tsim_lt<int32_t>,
        .u32_fcn = tsim_lt<uint32_t>,
        .bool_fcn = nullptr
    };
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

tmdl::stdlib::RelationalBase::FunctionTypes tmdl::stdlib::LessThanEqual::get_application_functions() const
{
    return FunctionTypes
    {
        .double_fcn = tsim_leq<double>,
        .float_fcn = tsim_leq<float>,
        .i32_fcn = tsim_leq<int32_t>,
        .u32_fcn = tsim_leq<uint32_t>,
        .bool_fcn = nullptr
    };
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

tmdl::stdlib::RelationalBase::FunctionTypes tmdl::stdlib::Equal::get_application_functions() const
{
    return FunctionTypes
    {
        .double_fcn = tsim_eq<double>,
        .float_fcn = tsim_eq<float>,
        .i32_fcn = tsim_eq<int32_t>,
        .u32_fcn = tsim_eq<uint32_t>,
        .bool_fcn = tsim_eq<bool>
    };
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

tmdl::stdlib::RelationalBase::FunctionTypes tmdl::stdlib::NotEqual::get_application_functions() const
{
    return FunctionTypes
    {
        .double_fcn = tsim_neq<double>,
        .float_fcn = tsim_neq<float>,
        .i32_fcn = tsim_neq<int32_t>,
        .u32_fcn = tsim_neq<uint32_t>,
        .bool_fcn = tsim_neq<bool>
    };
}
