// SPDX-License-Identifier: GPL-3.0-only

#include "clock.hpp"
#include "../model_exception.hpp"

#include <memory>

#include <fmt/format.h>

#include "tmdlstd/clock.hpp"

struct ClockComponent : public tmdl::codegen::CodeComponent
{
    virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
    {
        return {};
    }

    virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
    {
        return tmdl::codegen::InterfaceDefinition("s_out", {"val"});
    }

    virtual std::string get_include_file_name() const override
    {
        return "tmdlstd/clock.hpp";
    }

    virtual std::string get_name_base() const override
    {
        return "clock_block";
    }

    virtual std::string get_type_name() const override
    {
        return "tmdlstd::clock_block";
    }

    virtual std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override
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
};

class ClockExecutor : public tmdl::BlockExecutionInterface
{
public:
    ClockExecutor(
        std::shared_ptr<tmdl::ModelValueBox<tmdl::DataType::DOUBLE>> ptr_output) :
        output_value(ptr_output),
        block(nullptr)
    {
        if (ptr_output == nullptr)
        {
            throw tmdl::ModelException("input pointers cannot be null");
        }
    }

public:
    void init(const tmdl::SimState& s) override
    {
        block = std::make_unique<tmdl::stdlib::clock_block>(s.get_dt());
    }

    void step(const tmdl::SimState&) override
    {
        block->step();
        output_value->value = block->s_out.val;
    }

    void reset(const tmdl::SimState&) override
    {
        block->reset();
    }

    void close() override
    {
        block = nullptr;
    }

protected:
    std::shared_ptr<tmdl::ModelValueBox<tmdl::DataType::DOUBLE>> output_value;
    std::unique_ptr<tmdl::stdlib::clock_block> block;
};


tmdl::blocks::Clock::Clock()
{
    // Empty Constructor
}

std::string tmdl::blocks::Clock::get_name() const
{
    return "clock";
}

std::string tmdl::blocks::Clock::get_description() const
{
    return "Clock to provide the current time in seconds";
}

size_t tmdl::blocks::Clock::get_num_inputs() const
{
    return 0;
}

size_t tmdl::blocks::Clock::get_num_outputs() const
{
    return 1;
}

bool tmdl::blocks::Clock::update_block()
{
    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::blocks::Clock::has_error() const
{
    return nullptr;
}

void tmdl::blocks::Clock::set_input_type(
    const size_t,
    const DataType)
{
    throw ModelException("no input ports for clock");
}

tmdl::DataType tmdl::blocks::Clock::get_output_type(const size_t port) const
{
    if (port == 0)
    {
        return DataType::DOUBLE;
    }
    else
    {
        throw ModelException("invalid output port requested");
    }
}

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::blocks::Clock::get_execution_interface(
    const ConnectionManager& /* connections */,
    const VariableManager& manager) const
{
    VariableIdentifier vid = {
        .block_id = get_id(),
        .output_port_num = 0
    };

    auto ptr = std::dynamic_pointer_cast<ModelValueBox<DataType::DOUBLE>>(manager.get_ptr(vid));

    return std::make_shared<ClockExecutor>(ptr);
}
