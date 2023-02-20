// SPDX-License-Identifier: GPL-3.0-only

#include "limiter.hpp"

#include "../model_exception.hpp"

#include <algorithm>

using namespace tmdl;
using namespace tmdl::stdlib;


template <tmdl::DataType DT>
class LimiterExecutor : public BlockExecutionInterface
{
protected:
    using limit_t = tmdl::data_type_t<DT>::type;

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
    void step(const SimState&) override
    {
        const limit_t current = _ptr_input->value;
        if (current < _val_min->value)
        {
            _ptr_output->value = _val_min->value;
        }
        else if (current > _val_max->value)
        {
            _ptr_output->value = _val_max->value;
        }
        else
        {
            _ptr_output->value = current;
        }
    }

protected:
    std::shared_ptr<const ModelValueBox<DT>> _ptr_input;
    std::shared_ptr<ModelValueBox<DT>> _ptr_output;
    std::shared_ptr<const ModelValueBox<DT>> _val_min;
    std::shared_ptr<const ModelValueBox<DT>> _val_max;
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

std::shared_ptr<BlockExecutionInterface> Limiter::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    if (has_error() != nullptr)
    {
        throw ModelException("cannot execute with incomplete input parameters");
    }

    std::shared_ptr<ModelValue> maxValue;
    std::shared_ptr<ModelValue> minValue;

    if (dynamicLimiter->get_value().value.tf)
    {
        maxValue = manager.get_ptr(*connections.get_connection_to(get_id(), 1));
        minValue = manager.get_ptr(*connections.get_connection_to(get_id(), 2));
    }
    else
    {
        maxValue = prmMaxValue->get_value().to_box();
        minValue = prmMinValue->get_value().to_box();
    }

    const auto inputPointer = manager.get_ptr(*connections.get_connection_to(get_id(), 0));

    const auto vidOutput = VariableIdentifier {
        .block_id = get_id(),
        .output_port_num = 0
    };

    const auto outputPointer = manager.get_ptr(vidOutput);

    switch (input_type)
    {
    case DataType::DOUBLE:
        return std::make_shared<LimiterExecutor<DataType::DOUBLE>>(
            inputPointer,
            minValue,
            maxValue,
            outputPointer);
    case DataType::SINGLE:
        return std::make_shared<LimiterExecutor<DataType::SINGLE>>(
            inputPointer,
            minValue,
            maxValue,
            outputPointer);
    case DataType::INT32:
        return std::make_shared<LimiterExecutor<DataType::INT32>>(
            inputPointer,
            minValue,
            maxValue,
            outputPointer);
    case DataType::UINT32:
        return std::make_shared<LimiterExecutor<DataType::UINT32>>(
            inputPointer,
            minValue,
            maxValue,
            outputPointer);
    default:
        throw ModelException("unable to generate limitor executor");
    }
}
