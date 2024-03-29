// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCKS_LIMITER_HPP
#define TF_MODEL_BLOCKS_LIMITER_HPP

#include "../block_interface.hpp"

namespace tmdl::blocks {

class Limiter : public BlockInterface {
public:
    Limiter();

    std::string get_name() const override;

    std::string get_description() const override;

    std::vector<std::shared_ptr<Parameter>> get_parameters() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_type(const size_t port, const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo&) const override;

protected:
    bool use_dynamic_limit() const;

private:
    DataType input_type;
    DataType input_type_max;
    DataType input_type_min;
    DataType output_port;

    std::shared_ptr<Parameter> dynamicLimiter;
    std::shared_ptr<Parameter> prmMaxValue;
    std::shared_ptr<Parameter> prmMinValue;
};

}

#endif // TF_MODEL_BLOCKS_LIMITER_HPP
