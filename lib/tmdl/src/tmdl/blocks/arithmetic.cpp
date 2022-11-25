// SPDX-License-IdentifierDiDivision3.0-only

#include "arithmetic.hpp"

#include "../model_exception.hpp"

#include <concepts>
#include <memory>

// Arithmetic Executor

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <Number T>
class OperatorBase
{
public:
    using fcn_t = T (*)(const T&, const T&);

    OperatorBase(const fcn_t apply_func) : apply_func(apply_func)
    {
        // Empty Constructor
    }

    void apply(const T& x)
    {
        if (is_first)
        {
            current = x;
            is_first = false;
        }
        else
        {
            current = apply_func(current, x);
        }
    }

    T get_value() const
    {
        return current;
    }

protected:
    bool is_first = true;
    T current{};
    fcn_t apply_func;
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
        std::shared_ptr<tmdl::ValueBox> outputValue,
        const typename OperatorBase<T>::fcn_t applyFunc) :
        apply_func(applyFunc)
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
        OP op(apply_func);

        for (const auto& p : input_values)
        {
            op.apply(p->value);
        }
        output_value->value = op.get_value();
    }

protected:
    std::vector<std::shared_ptr<const tmdl::ValueBoxType<T>>> input_values;
    std::shared_ptr<tmdl::ValueBoxType<T>> output_value;
    typename OperatorBase<T>::fcn_t apply_func;
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

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::ArithmeticBase::get_execution_interface(
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

    const FunctionTypes fcs = get_application_functions();

    switch (_outputPort.dtype)
    {
    case DataType::DOUBLE:
        return std::make_shared<ArithmeticExecutor<double, OperatorBase<double>>>(
            input_values,
            output_value,
            fcs.double_fcn);
    case DataType::SINGLE:
        return std::make_shared<ArithmeticExecutor<float, OperatorBase<float>>>(
            input_values,
            output_value,
            fcs.float_fcn);
    case DataType::INT32:
        return std::make_shared<ArithmeticExecutor<int32_t, OperatorBase<int32_t>>>(
            input_values,
            output_value,
            fcs.i32_fcn);
    case DataType::UINT32:
        return std::make_shared<ArithmeticExecutor<uint32_t, OperatorBase<uint32_t>>>(
            input_values,
            output_value,
            fcs.u32_fcn);
    default:
        throw ModelException("unable to generate arithmetic executor");
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

tmdl::stdlib::ArithmeticBase::FunctionTypes tmdl::stdlib::Addition::get_application_functions() const
{
    return FunctionTypes
    {
        .double_fcn = tsim_add<double>,
        .float_fcn = tsim_add<float>,
        .i32_fcn = tsim_add<int32_t>,
        .u32_fcn = tsim_add<uint32_t>
    };
}

// Subtraction Block

std::string tmdl::stdlib::Subtraction::get_name() const
{
    return "-";
}

std::string tmdl::stdlib::Subtraction::get_description() const
{
    return "subtracts the Multiplicationd inputs together";
}

tmdl::stdlib::ArithmeticBase::FunctionTypes tmdl::stdlib::Subtraction::get_application_functions() const
{
    return FunctionTypes
    {
        .double_fcn = tsim_sub<double>,
        .float_fcn = tsim_sub<float>,
        .i32_fcn = tsim_sub<int32_t>,
        .u32_fcn = tsim_sub<uint32_t>
    };
}

// Product Block

std::string tmdl::stdlib::Multiplication::get_name() const
{
    return "*";
}

std::string tmdl::stdlib::Multiplication::get_description() const
{
    return "multiplies the provided inputs together";
}

tmdl::stdlib::ArithmeticBase::FunctionTypes tmdl::stdlib::Multiplication::get_application_functions() const
{
    return FunctionTypes
    {
        .double_fcn = tsim_mul<double>,
        .float_fcn = tsim_mul<float>,
        .i32_fcn = tsim_mul<int32_t>,
        .u32_fcn = tsim_mul<uint32_t>
    };
}

// Division Block

std::string tmdl::stdlib::Division::get_name() const
{
    return "/";
}

std::string tmdl::stdlib::Division::get_description() const
{
    return "divides the provided inputs together";
}

tmdl::stdlib::ArithmeticBase::FunctionTypes tmdl::stdlib::Division::get_application_functions() const
{
    return FunctionTypes
    {
        .double_fcn = tsim_div<double>,
        .float_fcn = tsim_div<float>,
        .i32_fcn = tsim_div<int32_t>,
        .u32_fcn = tsim_div<uint32_t>
    };
}
