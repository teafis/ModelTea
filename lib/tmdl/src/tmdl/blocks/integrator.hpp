// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_STDLIB_INTEGRATOR_HPP
#define TF_MODEL_STDLIB_INTEGRATOR_HPP

#include "../library.hpp"

namespace tmdl::stdlib
{

class Integrator : public LibraryBlock
{
public:
    Integrator();

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    std::vector<Parameter*> get_parameters() const override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_port(
        const size_t port,
        const DataType type) override;

    PortValue get_output_port(const size_t port) const override;

    std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const override;

protected:
    DataType input_type;
    DataType input_reset_flat_type;
    DataType input_reset_value_type;
    PortValue output_port;
};

}

#endif // TF_MODEL_STDLIB_INTEGRATOR_HPP
