// SPDX-License-Identifier: GPL-3.0-only

#include "arithmetic.hpp"

#include "../model_exception.hpp"

#include <concepts>
#include <memory>

// Arithmetic Executor

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <Number T, T (FCN)(const T&, const T&)>
class OperatorBase
{
public:
    void apply(const T& x)
    {
        if (is_first)
        {
            current = x;
            is_first = false;
        }
        else
        {
            current = FCN(current, x);
        }
    }

    T get_value() const
    {
        return current;
    }

protected:
    bool is_first = true;
    T current{};
};

template <Number T>
T tsim_add(const T& a, const T& b)
{
    return a + b;
}

template <Number T>
T tsim_sub(const T& a, const T& b)
{
    return a - b;
}

template <Number T>
T tsim_mul(const T& a, const T& b)
{
    return a * b;
}

template <Number T>
T tsim_div(const T& a, const T& b)
{
    return a / b;
}

template <Number T>
class AdditionOperator : public OperatorBase<T, tsim_add>
{
};

template <Number T>
class SubtractionOperator : public OperatorBase<T, tsim_sub>
{
};

template <Number T>
class MultiplicationOperator : public OperatorBase<T, tsim_mul>
{
};

template <Number T>
class DivisionOperator : public OperatorBase<T, tsim_div>
{
};

template <typename OP, typename N>
concept OperatorClass = requires(OP op, N n) {
    requires Number<N>;
    { op.get_value() } -> std::same_as<N>;
    op.apply(n);
};

template <Number T, OperatorClass<T> OP>
struct ArithmeticExecutor : public tmdl::BlockExecutionInterface
{
    ArithmeticExecutor(
        std::vector<std::shared_ptr<const tmdl::ValueBox>> inputValues,
        std::shared_ptr<tmdl::ValueBox> outputValue)
    {
        output_value = std::dynamic_pointer_cast<tmdl::ValueBoxType<T>>(outputValue);

        if (output_value == nullptr)
        {
            throw tmdl::ModelException("output pointer must be non-null");
        }

        for (const auto& p : inputValues)
        {
            const auto ptr = std::dynamic_pointer_cast<const tmdl::ValueBoxType<T>>(p);
            if (ptr == nullptr)
            {
                throw tmdl::ModelException("input pointer must be non-null");
            }

            input_values.push_back(ptr);
        }
    }

    void step(const tmdl::SimState&) override
    {
        OP op;

        for (const auto& p : input_values)
        {
            op.apply(p->value);
        }
        output_value->value = op.get_value();
    }

protected:
    std::vector<std::shared_ptr<const tmdl::ValueBoxType<T>>> input_values;
    std::shared_ptr<tmdl::ValueBoxType<T>> output_value;
};

// Arithmetic Base

tmdl::stdlib::ArithmeticBase::ArithmeticBase() :
    _prmNumInputPorts(std::make_unique<Parameter>("num_inputs", "number of input ports", ParameterValue::from_string("1", ParameterValue::Type::UINT32)))
{
    _inputPorts.resize(currentPrmPortCount());
}

size_t tmdl::stdlib::ArithmeticBase::get_num_inputs() const
{
    return currentPrmPortCount();
}

size_t tmdl::stdlib::ArithmeticBase::get_num_outputs() const
{
    return 1;
}

std::vector<tmdl::Parameter*> tmdl::stdlib::ArithmeticBase::get_parameters() const
{
    return { _prmNumInputPorts.get() };
}

bool tmdl::stdlib::ArithmeticBase::update_block()
{
    auto firstType = DataType::UNKNOWN;

    bool updated = false;

    if (_inputPorts.size() != get_num_inputs())
    {
        _inputPorts.resize(get_num_inputs());
        updated = true;
    }

    if (_inputPorts.size() > 0)
    {
        firstType = _inputPorts[0].dtype;
    }

    if (_outputPort.dtype != firstType)
    {
        _outputPort.dtype = firstType;
        updated = true;
    }

    return updated;
}

std::unique_ptr<const tmdl::BlockError> tmdl::stdlib::ArithmeticBase::has_error() const
{
    if (currentPrmPortCount() < 1)
    {
        return std::make_unique<BlockError>(BlockError
        {
            .id = get_id(),
            .message = "arithmetic block must have >= 2 ports"
        });
    }

    const DataType firstType = _inputPorts[0].dtype;

    for (const auto& p : _inputPorts)
    {
        if (p.dtype == DataType::BOOLEAN || p.dtype == DataType::UNKNOWN)
        {
            return std::make_unique<BlockError>(BlockError
            {
                .id = get_id(),
                .message = "unknown data type provided for input port"
            });
        }
        else if (p.dtype != firstType)
        {
            return std::make_unique<BlockError>(BlockError
            {
                .id = get_id(),
                .message = "all input ports must have the same data type"
            });
        }
    }

    return nullptr;
}

void tmdl::stdlib::ArithmeticBase::set_input_port(
    const size_t port,
    const PortValue value)
{
    _inputPorts.resize(currentPrmPortCount());

    if (port < _inputPorts.size())
    {
        _inputPorts[port] = value;
    }
    else
    {
        throw ModelException("provided port value exceeds input port count");
    }
}

tmdl::PortValue tmdl::stdlib::ArithmeticBase::get_output_port(const size_t port) const
{
    if (port == 0)
    {
        DataType dtype = DataType::UNKNOWN;
        if (_inputPorts.size() > 0)
        {
            dtype = _inputPorts[0].dtype;
        }

        return PortValue
        {
            .dtype = dtype
        };
    }
    else
    {
        throw ModelException("output port is out of range");
    }
}

size_t tmdl::stdlib::ArithmeticBase::currentPrmPortCount() const
{
    return _prmNumInputPorts->get_value().value.u32;
}

// Addition Block

std::string tmdl::stdlib::Addition::get_name() const
{
    return "+";
}

std::string tmdl::stdlib::Addition::get_description() const
{
    return "adds the provided inputs together";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Addition::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot generate a model with an error");
    }

    std::vector<std::shared_ptr<const ValueBox>> input_values;
    for (size_t i = 0; i < _inputPorts.size(); ++i)
    {
        const auto& c = connections.get_connection_to(get_id(), i);
        input_values.push_back(manager.get_ptr(c));
    }

    auto output_value = manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (_outputPort.dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<ArithmeticExecutor<double, AdditionOperator<double>>>(
            input_values,
            output_value);
    case DataType::SINGLE:
        return std::make_shared<ArithmeticExecutor<float, AdditionOperator<float>>>(
            input_values,
            output_value);
    case DataType::INT32:
        return std::make_shared<ArithmeticExecutor<int32_t, AdditionOperator<int32_t>>>(
            input_values,
            output_value);
    case DataType::UINT32:
        return std::make_shared<ArithmeticExecutor<uint32_t, AdditionOperator<uint32_t>>>(
            input_values,
            output_value);
    default:
        throw ModelException("unable to generate addition executor");
    }
}

// Subtraction Block

std::string tmdl::stdlib::Subtraction::get_name() const
{
    return "-";
}

std::string tmdl::stdlib::Subtraction::get_description() const
{
    return "subtracts the provided inputs together";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Subtraction::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot generate a model with an error");
    }

    std::vector<std::shared_ptr<const ValueBox>> input_values;
    for (size_t i = 0; i < _inputPorts.size(); ++i)
    {
        const auto& c = connections.get_connection_to(get_id(), i);
        input_values.push_back(manager.get_ptr(c));
    }

    auto output_value = manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (_outputPort.dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<ArithmeticExecutor<double, SubtractionOperator<double>>>(
            input_values,
            output_value);
    case DataType::SINGLE:
        return std::make_shared<ArithmeticExecutor<float, SubtractionOperator<float>>>(
            input_values,
            output_value);
    case DataType::INT32:
        return std::make_shared<ArithmeticExecutor<int32_t, SubtractionOperator<int32_t>>>(
            input_values,
            output_value);
    case DataType::UINT32:
        return std::make_shared<ArithmeticExecutor<uint32_t, SubtractionOperator<uint32_t>>>(
            input_values,
            output_value);
    default:
        throw ModelException("unable to generate subtraction executor");
    }
}

// Product Block

std::string tmdl::stdlib::Product::get_name() const
{
    return "*";
}

std::string tmdl::stdlib::Product::get_description() const
{
    return "multiplies the provided inputs together";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Product::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot generate a model with an error");
    }

    std::vector<std::shared_ptr<const ValueBox>> input_values;
    for (size_t i = 0; i < _inputPorts.size(); ++i)
    {
        const auto& c = connections.get_connection_to(get_id(), i);
        input_values.push_back(manager.get_ptr(c));
    }

    auto output_value = manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (_outputPort.dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<ArithmeticExecutor<double, MultiplicationOperator<double>>>(
            input_values,
            output_value);
    case DataType::SINGLE:
        return std::make_shared<ArithmeticExecutor<float, MultiplicationOperator<float>>>(
            input_values,
            output_value);
    case DataType::INT32:
        return std::make_shared<ArithmeticExecutor<int32_t, MultiplicationOperator<int32_t>>>(
            input_values,
            output_value);
    case DataType::UINT32:
        return std::make_shared<ArithmeticExecutor<uint32_t, MultiplicationOperator<uint32_t>>>(
            input_values,
            output_value);
    default:
        throw ModelException("unable to generate product executor");
    }
}

// Division Block

std::string tmdl::stdlib::Divide::get_name() const
{
    return "/";
}

std::string tmdl::stdlib::Divide::get_description() const
{
    return "divides the provided inputs together";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Divide::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot generate a model with an error");
    }

    std::vector<std::shared_ptr<const ValueBox>> input_values;
    for (size_t i = 0; i < _inputPorts.size(); ++i)
    {
        const auto& c = connections.get_connection_to(get_id(), i);
        input_values.push_back(manager.get_ptr(c));
    }

    auto output_value = manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    });

    switch (_outputPort.dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<ArithmeticExecutor<double, DivisionOperator<double>>>(
            input_values,
            output_value);
    case DataType::SINGLE:
        return std::make_shared<ArithmeticExecutor<float, DivisionOperator<float>>>(
            input_values,
            output_value);
    case DataType::INT32:
        return std::make_shared<ArithmeticExecutor<int32_t, DivisionOperator<int32_t>>>(
            input_values,
            output_value);
    case DataType::UINT32:
        return std::make_shared<ArithmeticExecutor<uint32_t, DivisionOperator<uint32_t>>>(
            input_values,
            output_value);
    default:
        throw ModelException("unable to generate division executor");
    }
}
