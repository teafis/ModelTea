// SPDX-License-Identifier: GPL-3.0-only

#include "clock.hpp"

#include "../model_exception.hpp"

class ClockExecutor : public tmdl::BlockExecutionInterface
{
public:
    ClockExecutor(
        std::shared_ptr<tmdl::ValueBoxType<double>> ptr_output) :
        output_value(ptr_output)
    {
        if (ptr_output == nullptr)
        {
            throw tmdl::ModelException("input pointers cannot be null");
        }
    }

public:
    void step(const tmdl::SimState& s) override
    {
        output_value->value = s.time;
    }

protected:
    std::shared_ptr<tmdl::ValueBoxType<double>> output_value;
};


tmdl::stdlib::Clock::Clock()
{
    // Empty Constructor
}

std::string tmdl::stdlib::Clock::get_name() const
{
    return "clock";
}

std::string tmdl::stdlib::Clock::get_description() const
{
    return "Clock to provide the current time in seconds";
}

size_t tmdl::stdlib::Clock::get_num_inputs() const
{
    return 0;
}

size_t tmdl::stdlib::Clock::get_num_outputs() const
{
    return 1;
}

bool tmdl::stdlib::Clock::update_block()
{
    return false;
}

std::unique_ptr<const tmdl::BlockError> tmdl::stdlib::Clock::has_error() const
{
    return nullptr;
}

void tmdl::stdlib::Clock::set_input_type(
    const size_t,
    const DataType)
{
    throw ModelException("no input ports for clock");
}

tmdl::DataType tmdl::stdlib::Clock::get_output_type(const size_t port) const
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

std::shared_ptr<tmdl::BlockExecutionInterface> tmdl::stdlib::Clock::get_execution_interface(
    const ConnectionManager& /* connections */,
    const VariableManager& manager) const
{
    VariableIdentifier vid = {
        .block_id = get_id(),
        .output_port_num = 0
    };

    auto ptr = std::dynamic_pointer_cast<ValueBoxType<double>>(manager.get_ptr(vid));

    return std::make_shared<ClockExecutor>(ptr);
}
