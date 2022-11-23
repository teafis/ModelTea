// SPDX-License-Identifier: GPL-3.0-only

#include "arithmetic.hpp"

#include "../model_exception.hpp"

#include <memory>

// Arithmetic Base

tmdl::stdlib::ArithmeticBase::ArithmeticBase() :
    _prmNumInputPorts(std::make_unique<Parameter>("num_inputs", "number of input ports", ParameterValue::from_string("1", ParameterValue::Type::UINT32)))
{
    _inputPorts.resize(currentPrmPortCount());
}

size_t tmdl::stdlib::ArithmeticBase::get_num_inputs() const
{
    return _inputPorts.size();
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

    if (_inputPorts.size() > 0)
    {
        firstType = _inputPorts[0].dtype;
    }

    if (_outputPort.dtype != firstType)
    {
        _outputPort.dtype = firstType;
        return true;
    }

    return false;
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

    if (currentPrmPortCount() != _inputPorts.size())
    {
        return std::make_unique<BlockError>(BlockError
        {
            .id = get_id(),
            .message = "mismatch in provided output values"
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

// Arithmetic Executor

template <typename T>
struct ArithOperator
{
    virtual void apply(const T& x) = 0;
    virtual T get_value() const = 0;
};

template <typename T>
class AdditionOperator : public ArithOperator<T>
{
public:
    AdditionOperator() : current(static_cast<T>(0))
    {
        // Empty Constructor
    }

    void apply(const T& x) override
    {
        current += x;
    }

    T get_value() const override
    {
        return current;
    }

protected:
    T current;
};

template <typename T>
class SubtractionOperator : public ArithOperator<T>
{
public:
    SubtractionOperator() : current(static_cast<T>(0))
    {
        // Empty Constructor
    }

    void apply(const T& x) override
    {
        if (is_first)
        {
            current = x;
            is_first = false;
        }
        else
        {
            current -= x;
        }
    }

    T get_value() const override
    {
        return current;
    }

protected:
    bool is_first = false;
    T current;
};

template <typename T>
class MultiplicationOperator : public ArithOperator<T>
{
public:
    MultiplicationOperator() : current(static_cast<T>(1))
    {
        // Empty Constructor
    }

    void apply(const T& x) override
    {
        current *= x;
    }

    T get_value() const override
    {
        return current;
    }

protected:
    T current;
};

template <typename T>
class DivisionOperator : public ArithOperator<T>
{
public:
    DivisionOperator() : current(static_cast<T>(0))
    {
        // Empty Constructor
    }

    void apply(const T& x) override
    {
        if (is_first)
        {
            current = x;
            is_first = false;
        }
        else
        {
            current /= x;
        }
    }

    T get_value() const override
    {
        return current;
    }

protected:
    bool is_first = false;
    T current;
};

template <template <typename> class OP, class T>
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
        OP<T> op;
        ArithOperator<T>* ptr = &op;

        for (const auto& p : input_values)
        {
            ptr->apply(p->value);
        }
        output_value->value = ptr->get_value();
    }

protected:
    std::vector<std::shared_ptr<const tmdl::ValueBoxType<T>>> input_values;
    std::shared_ptr<tmdl::ValueBoxType<T>> output_value;
};

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
        return std::make_shared<ArithmeticExecutor<AdditionOperator, double>>(
            input_values,
            output_value);
    case DataType::SINGLE:
        return std::make_shared<ArithmeticExecutor<AdditionOperator, float>>(
            input_values,
            output_value);
    case DataType::INT32:
        return std::make_shared<ArithmeticExecutor<AdditionOperator, int32_t>>(
            input_values,
            output_value);
    case DataType::UINT32:
        return std::make_shared<ArithmeticExecutor<AdditionOperator, uint32_t>>(
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
        return std::make_shared<ArithmeticExecutor<SubtractionOperator, double>>(
            input_values,
            output_value);
    case DataType::SINGLE:
        return std::make_shared<ArithmeticExecutor<SubtractionOperator, float>>(
            input_values,
            output_value);
    case DataType::INT32:
        return std::make_shared<ArithmeticExecutor<SubtractionOperator, int32_t>>(
            input_values,
            output_value);
    case DataType::UINT32:
        return std::make_shared<ArithmeticExecutor<SubtractionOperator, uint32_t>>(
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
        return std::make_shared<ArithmeticExecutor<MultiplicationOperator, double>>(
            input_values,
            output_value);
    case DataType::SINGLE:
        return std::make_shared<ArithmeticExecutor<MultiplicationOperator, float>>(
            input_values,
            output_value);
    case DataType::INT32:
        return std::make_shared<ArithmeticExecutor<MultiplicationOperator, int32_t>>(
            input_values,
            output_value);
    case DataType::UINT32:
        return std::make_shared<ArithmeticExecutor<MultiplicationOperator, uint32_t>>(
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
        return std::make_shared<ArithmeticExecutor<DivisionOperator, double>>(
            input_values,
            output_value);
    case DataType::SINGLE:
        return std::make_shared<ArithmeticExecutor<DivisionOperator, float>>(
            input_values,
            output_value);
    case DataType::INT32:
        return std::make_shared<ArithmeticExecutor<DivisionOperator, int32_t>>(
            input_values,
            output_value);
    case DataType::UINT32:
        return std::make_shared<ArithmeticExecutor<DivisionOperator, uint32_t>>(
            input_values,
            output_value);
    default:
        throw ModelException("unable to generate division executor");
    }
}
