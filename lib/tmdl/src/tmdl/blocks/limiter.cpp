// SPDX-License-Identifier: GPL-3.0-only

#include "limiter.hpp"

#include "../model_exception.hpp"

#include <algorithm>

#include <fmt/format.h>

#include <tmdlstd/tmdlstd.hpp>

using namespace tmdl;
using namespace tmdl::blocks;

struct LimiterValues
{
    std::shared_ptr<tmdl::ModelValue> upper;
    std::shared_ptr<tmdl::ModelValue> lower;
};

template <tmdl::DataType DT>
class CompiledLimiter : public tmdl::CompiledBlockInterface
{
protected:
    using limit_t = typename tmdl::data_type_t<DT>::type;

public:
    CompiledLimiter(const size_t id, const std::optional<LimiterValues> constLimits) :
        _id{ id },
        _constantLimits{ constLimits }
    {
        // Empty Constructor
    }

    std::shared_ptr<tmdl::BlockExecutionInterface> get_execution_interface(
        const tmdl::ConnectionManager& connections,
        const tmdl::VariableManager& manager) const override
    {
        std::shared_ptr<ModelValue> maxValue;
        std::shared_ptr<ModelValue> minValue;

        if (_constantLimits)
        {
            maxValue = _constantLimits->upper;
            minValue = _constantLimits->lower;
        }
        else
        {
            maxValue = manager.get_ptr(*connections.get_connection_to(_id, 1));
            minValue = manager.get_ptr(*connections.get_connection_to(_id, 2));
        }

        const auto inputPointer = manager.get_ptr(*connections.get_connection_to(_id, 0));

        const auto vidOutput = VariableIdentifier {
            .block_id = _id,
            .output_port_num = 0
        };

        const auto outputPointer = manager.get_ptr(vidOutput);

        return std::make_shared<LimiterExecutor>(
            inputPointer,
            minValue,
            maxValue,
            outputPointer);
    }

    std::unique_ptr<tmdl::codegen::CodeComponent> get_codegen_self() const override
    {
        if (_constantLimits)
        {
            return std::make_unique<LimiterComponentConst>(
                tmdl::ModelValue::get_inner_value<DT>(_constantLimits->upper.get()),
                tmdl::ModelValue::get_inner_value<DT>(_constantLimits->lower.get()));
        }
        else
        {
            return std::make_unique<LimiterComponent>();
        }
    }

protected:
    const size_t _id;
    const std::optional<LimiterValues> _constantLimits;

protected:
    struct LimiterComponent : public tmdl::codegen::CodeComponent
    {
        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_in", {"input_value", "limit_upper", "limit_lower"});
        }

        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_out", {"output_value"});
        }

        virtual std::string get_module_name() const override
        {
            return "tmdlstd/tmdlstd.hpp";
        }

        virtual std::string get_name_base() const override
        {
            return "limiter_block";
        }

        virtual std::string get_type_name() const override
        {
            return fmt::format("tmdl::stdlib::limiter_block<{}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT));
        }

        virtual std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override
        {
            switch (ft)
            {
            case tmdl::codegen::BlockFunction::INIT:
                return "init";
            case tmdl::codegen::BlockFunction::STEP:
                return "step";
            default:
                return {};
            }
        }
    };

    struct LimiterComponentConst : public tmdl::codegen::CodeComponent
    {
        LimiterComponentConst(const limit_t upper, const limit_t lower) : _upper{ upper }, _lower{ lower }
        {
            // Empty Constructor
        }

        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_in", {"input_value"});
        }

        virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
        {
            return tmdl::codegen::InterfaceDefinition("s_out", {"output_value"});
        }

        virtual std::string get_module_name() const override
        {
            return "tmdlstd/tmdlstd.hpp";
        }

        virtual std::string get_name_base() const override
        {
            return "limiter_block_const";
        }

        virtual std::string get_type_name() const override
        {
            return fmt::format("tmdl::stdlib::limiter_block_const<{}>", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, DT));
        }

        virtual std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override
        {
            switch (ft)
            {
            case tmdl::codegen::BlockFunction::STEP:
                return "step";
            case tmdl::codegen::BlockFunction::INIT:
                return "init";
            default:
                return {};
            }
        }

        virtual std::vector<std::string> constructor_arguments() const override
        {
            return {
                fmt::format("{}", _upper),
                fmt::format("{}", _lower),
            };
        }

        const limit_t _upper;
        const limit_t _lower;
    };

protected:
    class LimiterExecutor : public BlockExecutionInterface
    {
    public:
        LimiterExecutor(
            std::shared_ptr<const ModelValue> ptr_input,
            std::shared_ptr<const ModelValue> val_min,
            std::shared_ptr<const ModelValue> val_max,
            std::shared_ptr<ModelValue> ptr_output) :
            _ptr_input(std::dynamic_pointer_cast<const ModelValueBox<DT>>(ptr_input)),
            _ptr_output(std::dynamic_pointer_cast<ModelValueBox<DT>>(ptr_output)),
            _val_min(std::dynamic_pointer_cast<const ModelValueBox<DT>>(val_min)),
            _val_max(std::dynamic_pointer_cast<const ModelValueBox<DT>>(val_max))
        {
            if (_ptr_input == nullptr || _ptr_output == nullptr || _val_min == nullptr || _val_max == nullptr)
            {
                throw ModelException("input pointers cannot be null");
            }
        }

    public:
        void init() override
        {
            block.s_in.input_value = _ptr_input->value;
            block.s_in.limit_lower = _val_min->value;
            block.s_in.limit_upper = _val_max->value;
            block.init();
            _ptr_output->value = block.s_out.output_value;
        }

        void step() override
        {
            block.s_in.input_value = _ptr_input->value;
            block.s_in.limit_lower = _val_min->value;
            block.s_in.limit_upper = _val_max->value;
            block.step();
            _ptr_output->value = block.s_out.output_value;
        }

    protected:
        std::shared_ptr<const ModelValueBox<DT>> _ptr_input;
        std::shared_ptr<ModelValueBox<DT>> _ptr_output;
        std::shared_ptr<const ModelValueBox<DT>> _val_min;
        std::shared_ptr<const ModelValueBox<DT>> _val_max;

        tmdl::stdlib::limiter_block<limit_t> block;
    };
};

Limiter::Limiter()
{
    // Setup parameters
    dynamicLimiter = std::make_shared<Parameter>(
        "dynamic_limiter",
        "Use Dynamic Limits",
        ParameterValue
        {
            .dtype = ParameterValue::Type::BOOLEAN,
            .value = ParameterValue::Value
            {
                .tf = false
            }
        });

    prmMaxValue = std::make_shared<Parameter>(
        "max_value",
        "Maximum Value",
        ParameterValue{});

    prmMinValue = std::make_shared<Parameter>(
        "min_value",
        "Minimum Value",
        ParameterValue{});

    input_type = DataType::UNKNOWN;
    output_port = DataType::UNKNOWN;
    input_type_min = DataType::UNKNOWN;
    input_type_max = DataType::UNKNOWN;
}

std::string Limiter::get_name() const
{
    return "limiter";
}

std::string Limiter::get_description() const
{
    return "Limits input values by the provided parameters";
}

std::vector<std::shared_ptr<Parameter>> Limiter::get_parameters() const
{
    return {
        dynamicLimiter,
        prmMinValue,
        prmMaxValue
    };
}

size_t Limiter::get_num_inputs() const
{
    if (dynamicLimiter->get_value().value.tf)
    {
        return 3;
    }
    else
    {
        return 1;
    }
}

size_t Limiter::get_num_outputs() const
{
    return 1;
}

bool Limiter::update_block()
{
    bool updated = false;

    prmMaxValue->set_enabled(!dynamicLimiter->get_value().value.tf);
    prmMinValue->set_enabled(!dynamicLimiter->get_value().value.tf);

    if (input_type != output_port)
    {
        ParameterValue::Type new_dtype = ParameterValue::Type::UNKNOWN;

        switch (input_type)
        {
        case DataType::DOUBLE:
            new_dtype = ParameterValue::Type::DOUBLE;
            break;
        case DataType::SINGLE:
            new_dtype = ParameterValue::Type::SINGLE;
            break;
        case DataType::UINT32:
            new_dtype = ParameterValue::Type::UINT32;
            break;
        case DataType::INT32:
            new_dtype = ParameterValue::Type::INT32;
            break;
        default:
            break;
        }

        if (new_dtype != ParameterValue::Type::UNKNOWN)
        {
            prmMaxValue->get_value().convert(new_dtype);
            prmMinValue->get_value().convert(new_dtype);
        }

        output_port = input_type;
        updated = true;
    }

    return updated;
}

std::unique_ptr<const BlockError> Limiter::has_error() const
{
    const std::vector<DataType> supportedDataTypes = {
        DataType::DOUBLE,
        DataType::SINGLE,
        DataType::INT32,
        DataType::UINT32
    };

    if (std::find(supportedDataTypes.begin(), supportedDataTypes.end(), input_type) == supportedDataTypes.end())
    {
        return make_error("input port not set");
    }

    if (dynamicLimiter->get_value().value.tf)
    {
        if (input_type_max != input_type || input_type_min != input_type)
        {
            return make_error("min/max data types should be the same data type as the primary input");
        }
    }
    else
    {
        for (const auto& p : get_parameters())
        {
            if (p->get_value().dtype == ParameterValue::Type::UNKNOWN)
            {
                return make_error("min or max value is not able to be set");
            }
        }
    }

    return nullptr;
}

void Limiter::set_input_type(
    const size_t port,
    const DataType type)
{
    if (port < get_num_inputs())
    {
        switch (port)
        {
        case 0:
            input_type = type;
            break;
        case 1:
            input_type_min = type;
            break;
        case 2:
            input_type_max = type;
            break;
        }
    }
    else
    {
        throw ModelException("provided input port too high");
    }
}

DataType Limiter::get_output_type(const size_t port) const
{
    if (port < get_num_outputs())
    {
        return output_port;
    }
    else
    {
        throw ModelException("provided output port is too high");
    }
}

std::unique_ptr<CompiledBlockInterface> Limiter::get_compiled(const ModelInfo&) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot execute limiter with incomplete input parameters");
    }

    std::optional<LimiterValues> limitValues{};

    if (!dynamicLimiter->get_value().value.tf)
    {
        limitValues = LimiterValues{
            .upper = prmMaxValue->get_value().to_box(),
            .lower = prmMinValue->get_value().to_box()
        };
    }

    switch (input_type)
    {
    case DataType::DOUBLE:
        return std::make_unique<CompiledLimiter<DataType::DOUBLE>>(get_id(), limitValues);
    case DataType::SINGLE:
        return std::make_unique<CompiledLimiter<DataType::SINGLE>>(get_id(), limitValues);
    case DataType::INT32:
        return std::make_unique<CompiledLimiter<DataType::INT32>>(get_id(), limitValues);
    case DataType::UINT32:
        return std::make_unique<CompiledLimiter<DataType::UINT32>>(get_id(), limitValues);
    default:
        throw ModelException("unable to generate limitor executor");
    }
}
