// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_CLOCK_HPP
#define TF_MODEL_STDLIB_CLOCK_HPP

#include <tmdl/library.hpp>

namespace tmdl::stdlib
{

class Clock : public LibraryBlock
{
public:
    Clock();

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    void set_input_port(
        const size_t,
        const PortValue*) override;

    const PortValue* get_output_port(const size_t port) const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface() const override;

protected:
    std::unique_ptr<PortValue> output_port;
    std::unique_ptr<ValueBox> output_port_value;
};

}

#endif // TF_MODEL_STDLIB_CLOCK_HPP
