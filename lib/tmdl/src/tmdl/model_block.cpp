// SPDX-License-Identifier: GPL-3.0-only

#include "model_block.hpp"

#include <functional>

#include "model_exception.hpp"

tmdl::ModelBlock::ModelBlock(std::shared_ptr<Model> model) :
    model(model)
{
    // Empty Constructor
}


std::string tmdl::ModelBlock::get_name() const
{
    return model->get_name();
}

std::string tmdl::ModelBlock::get_description() const
{
    return model->get_description();
}

size_t tmdl::ModelBlock::get_num_outputs() const
{
    return model->get_num_outputs();
}

size_t tmdl::ModelBlock::get_num_inputs() const
{
    return model->get_num_inputs();
}

bool tmdl::ModelBlock::update_block()
{
    bool updated = model->update_block();

    if (input_types.size() != get_num_inputs())
    {
        input_types.resize(get_num_inputs(), DataType::UNKNOWN);
        updated = true;
    }

    if (output_types.size() != get_num_outputs())
    {
        output_types.resize(get_num_outputs(), DataType::UNKNOWN);
        updated = true;
    }

    for (size_t i = 0; i < get_num_inputs(); ++i)
    {
        auto& dtype = input_types[i];
        const auto model_dtype = model->get_input_datatype(i);

        if (dtype != model_dtype)
        {
            dtype = model_dtype;
            updated = true;
        }
    }

    for (size_t i = 0; i < get_num_outputs(); ++i)
    {
        auto& dtype = output_types[i];
        const auto model_dtype = model->get_output_datatype(i);

        if (dtype != model_dtype)
        {
            dtype = model_dtype;
            updated = true;
        }
    }

    return updated;
}

std::unique_ptr<const tmdl::BlockError> tmdl::ModelBlock::has_error() const
{
    auto err = model->has_error();
    if (err != nullptr) return err;

    const std::vector<std::tuple<const std::vector<DataType>&, std::function<DataType (const size_t)>, std::function<size_t ()>>> portvec
    {
        { input_types, [this](const size_t port) { return model->get_input_datatype(port); }, [this]() { return get_num_inputs(); } },
        { output_types, [this](const size_t port) { return model->get_output_datatype(port); }, [this]() { return get_num_outputs(); } }
    };

    for (const auto& it : portvec)
    {
        const auto& [vec, dt_func, size_func] = it;
        //const auto& vec = std::get<0>(it);
        //const auto dt_func = std::get<1>(it);
        //const auto size_func = std::get<2>(it);

        if (vec.size() != size_func())
        {
            return make_error("model port size mismatch");
        }

        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (vec[i] != dt_func(i))
            {
                return make_error("model port type mismatch");
            }
        }
    }

    return nullptr;
}

void tmdl::ModelBlock::set_input_type(
    const size_t port,
    const DataType type)
{
    if (input_types.size() != get_num_inputs())
    {
        input_types.resize(get_num_inputs(), DataType::UNKNOWN);
    }

    if (port < input_types.size())
    {
        input_types[port] = type;
    }
    else
    {
        throw ModelException("input port out of range");
    }
}

tmdl::DataType tmdl::ModelBlock::get_output_type(const size_t port) const
{
    if (port < get_num_outputs())
    {
        if (port < output_types.size())
        {
            return output_types[port];
        }
        else
        {
            return DataType::UNKNOWN;
        }
    }
    else
    {
        throw ModelException("output port out of range");
    }
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::ModelBlock::get_execution_interface(
    const ConnectionManager& connections,
    const VariableManager& manager) const
{
    (void)connections;
    (void)manager;
    return nullptr;
}

std::shared_ptr<tmdl::Model> tmdl::ModelBlock::get_model()
{
    return model;
}

std::shared_ptr<const tmdl::Model> tmdl::ModelBlock::get_model() const
{
    return model;
}
