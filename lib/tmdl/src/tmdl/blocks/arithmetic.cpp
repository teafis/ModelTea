// SPDX-License-IdentifierDiDivision3.0-only

#include "arithmetic.hpp"

#include "../model_exception.hpp"

#include <concepts>
#include <memory>

#include <fmt/format.h>
#include <tmdlstd/arith.hpp>
#include <tmdlstd/util.hpp>
#include <tmdlstd/arith.hpp>
#include <tmdlstd/util.hpp>

// Arithmetic Executor

template <tmdl::DataType DT, tmdl::stdlib::ArithType OP>
struct ArithmeticExecutor : public tmdl::BlockExecutionInterface
{
    using type_t = typename tmdl::data_type_t<DT>::type;

    ArithmeticExecutor(
        std::vector<std::shared_ptr<const tmdl::ModelValue>> inputValues,
        std::shared_ptr<tmdl::ModelValue> outputValue)
    {
        output_value = std::dynamic_pointer_cast<tmdl::ModelValueBox<DT>>(outputValue);

        if (output_value == nullptr)
        {
            throw tmdl::ModelException("output pointer must be non-null");
        }

        for (const auto& p : inputValues)
        {
            const auto ptr = std::dynamic_pointer_cast<const tmdl::ModelValueBox<DT>>(p);
            if (ptr == nullptr)
            {
                throw tmdl::ModelException("input pointer must be non-null");
            }

            input_values.push_back(ptr);
            input_value_ptr_array.push_back(&ptr->value);
        }

        block.s_in.vals = input_value_ptr_array.data();
        block.s_in.size = input_value_ptr_array.size();
    }

    void step(const tmdl::SimState&) override
    {
        block.step();
        output_value->value = block.s_out.val;
    }

protected:
    struct ArithCodeComp : public tmdl::codegen::CodeComponent
    {
        ArithCodeComp(size_t size) : _size(size)
        {
            // Empty Constructor
        }

        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
        {
            std::vector<std::string> num_fields;

            for (size_t i = 0; i < _size; ++i)
            {
                num_fields.push_back(fmt::format("vals[{}]", i));
            }

            return tmdl::codegen::InterfaceDefinition("s_in", num_fields);
        }

        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_out", {"val"});
        }

        virtual std::string get_include_file_name() const override
        {
            return "tmdlstd/arith.hpp";
        }

        virtual std::string get_name_base() const override
        {
            return "arith_block";
        }

        virtual std::string get_type_name() const override
        {
            return fmt::format(
                "tmdlstd::arith_block<{}, {}, {}>",
                tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT),
                tmdl::stdlib::arith_to_string(OP),
                _size);
        }

        virtual std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const
        {
            if (ft == tmdl::codegen::BlockFunction::STEP)
            {
                return "step";
            }
            else
            {
                return {};
            }
        }

    protected:
        virtual std::vector<std::string> write_cpp_code(tmdl::codegen::CodeSection section) const override
        {
            (void)section;
            return {};
        }

        const size_t _size;
    };

public:
    std::unique_ptr<tmdl::codegen::CodeComponent> generate_code_component() const override
    {
        return std::make_unique<ArithCodeComp>(input_values.size());
    }

protected:
    std::vector<std::shared_ptr<const tmdl::ModelValueBox<DT>>> input_values;
    std::vector<const type_t*> input_value_ptr_array;
    std::shared_ptr<tmdl::ModelValueBox<DT>> output_value;

    tmdl::stdlib::arith_block_dynamic<type_t, OP> block;
};

// Arithmetic Base

tmdl::blocks::ArithmeticBase::ArithmeticBase() :
    _prmNumInputPorts(std::make_shared<Parameter>("num_inputs", "number of input ports", ParameterValue::from_string("2", ParameterValue::Type::UINT32)))
{
    _inputTypes.resize(currentPrmPortCount(), DataType::UNKNOWN);
}

size_t tmdl::blocks::ArithmeticBase::get_num_inputs() const
{
    return currentPrmPortCount();
}

size_t tmdl::blocks::ArithmeticBase::get_num_outputs() const
{
    return 1;
}

std::vector<std::shared_ptr<tmdl::Parameter>> tmdl::blocks::ArithmeticBase::get_parameters() const
{
    return { _prmNumInputPorts };
}

bool tmdl::blocks::ArithmeticBase::update_block()
{
    auto firstType = DataType::UNKNOWN;

    bool updated = false;

    if (_inputTypes.size() != get_num_inputs())
    {
        _inputTypes.resize(get_num_inputs(), DataType::UNKNOWN);
        updated = true;
    }

    for (const auto& t : _inputTypes)
    {
        if (t != DataType::UNKNOWN)
        {
            firstType = t;
            break;
        }
    }

    if (_outputPort != firstType)
    {
        _outputPort = firstType;
        updated = true;
    }

    return updated;
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::ArithmeticBase::has_error() const
{
    if (currentPrmPortCount() < 1)
    {
        return make_error("arithmetic block must have >= 2 ports");
    }

    const DataType firstType = _inputTypes[0];

    for (const auto& p : _inputTypes)
    {
        if (p == DataType::BOOLEAN || p == DataType::UNKNOWN)
        {
            return make_error("unknown data type provided for input port");
        }
        else if (p != firstType)
        {
            return make_error("all input ports must have the same data type");
        }
    }

    return nullptr;
}

void tmdl::blocks::ArithmeticBase::set_input_type(
    const size_t port,
    const DataType type)
{
    _inputTypes.resize(currentPrmPortCount());

    if (port < _inputTypes.size())
    {
        _inputTypes[port] = type;
    }
    else
    {
        throw ModelException("provided port value exceeds input port count");
    }
}

tmdl::DataType tmdl::blocks::ArithmeticBase::get_output_type(const size_t port) const
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

template <tmdl::stdlib::ArithType OP>
std::shared_ptr<tmdl::BlockExecutionInterface> generate_executor(
    const tmdl::DataType output_type,
    const std::vector<std::shared_ptr<const tmdl::ModelValue>>& input_values,
    const std::shared_ptr<tmdl::ModelValue> output_value)
{
    switch (output_type)
    {
    case tmdl::DataType::DOUBLE:
        return std::make_shared<ArithmeticExecutor<tmdl::DataType::DOUBLE, OP>>(
            input_values,
            output_value);
    case tmdl::DataType::SINGLE:
        return std::make_shared<ArithmeticExecutor<tmdl::DataType::SINGLE, OP>>(
            input_values,
            output_value);
    case tmdl::DataType::INT32:
        return std::make_shared<ArithmeticExecutor<tmdl::DataType::INT32, OP>>(
            input_values,
            output_value);
    case tmdl::DataType::UINT32:
        return std::make_shared<ArithmeticExecutor<tmdl::DataType::UINT32, OP>>(
            input_values,
            output_value);
    default:
        throw tmdl::ModelException("unable to generate arithmetic executor");
    }
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::blocks::ArithmeticBase::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot generate a model with an error");
    }

    std::vector<std::shared_ptr<const ModelValue>> input_values;
    for (size_t i = 0; i < _inputTypes.size(); ++i)
    {
        const auto c = connections.get_connection_to(get_id(), i);
        input_values.push_back(manager.get_ptr(*c));
    }

    auto output_value = manager.get_ptr(VariableIdentifier
    {
        .block_id = get_id(),
        .output_port_num = 0
    });

    return get_application_functions(input_values, output_value);
}

size_t tmdl::blocks::ArithmeticBase::currentPrmPortCount() const
{
    return _prmNumInputPorts->get_value().value.u32;
}

// Addition Block

std::string tmdl::blocks::Addition::get_name() const
{
    return "+";
}

std::string tmdl::blocks::Addition::get_description() const
{
    return "adds the provided inputs together";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::blocks::Addition::get_application_functions(
    const std::vector<std::shared_ptr<const ModelValue>>& input_values,
    const std::shared_ptr<tmdl::ModelValue> output_value) const
{
    return generate_executor<tmdl::stdlib::ArithType::ADD>(_outputPort, input_values, output_value);
}

// Subtraction Block

std::string tmdl::blocks::Subtraction::get_name() const
{
    return "-";
}

std::string tmdl::blocks::Subtraction::get_description() const
{
    return "subtracts the Multiplicationd inputs together";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::blocks::Subtraction::get_application_functions(
    const std::vector<std::shared_ptr<const ModelValue>>& input_values,
    const std::shared_ptr<tmdl::ModelValue> output_value) const
{
    return generate_executor<tmdl::stdlib::ArithType::SUB>(_outputPort, input_values, output_value);
}

// Product Block

std::string tmdl::blocks::Multiplication::get_name() const
{
    return "*";
}

std::string tmdl::blocks::Multiplication::get_description() const
{
    return "multiplies the provided inputs together";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::blocks::Multiplication::get_application_functions(
    const std::vector<std::shared_ptr<const ModelValue>>& input_values,
    const std::shared_ptr<tmdl::ModelValue> output_value) const
{
    return generate_executor<tmdl::stdlib::ArithType::MUL>(_outputPort, input_values, output_value);
}

// Division Block

std::string tmdl::blocks::Division::get_name() const
{
    return "/";
}

std::string tmdl::blocks::Division::get_description() const
{
    return "divides the provided inputs together";
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::blocks::Division::get_application_functions(
    const std::vector<std::shared_ptr<const ModelValue>>& input_values,
    const std::shared_ptr<tmdl::ModelValue> output_value) const
{
    return generate_executor<tmdl::stdlib::ArithType::DIV>(_outputPort, input_values, output_value);
}
