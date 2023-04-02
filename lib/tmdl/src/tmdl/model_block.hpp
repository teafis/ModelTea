// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCK_HPP
#define TF_MODEL_BLOCK_HPP

#include <memory>
#include <vector>

#include "block_interface.hpp"
#include "model.hpp"

#include <nlohmann/json.hpp>

namespace tmdl
{

class ModelBlock : public BlockInterface
{
public:
    ModelBlock(std::shared_ptr<Model> model);

    std::string get_name() const override;

    std::string get_description() const override;

    size_t get_num_inputs() const override;

    size_t get_num_outputs() const override;

    bool update_block() override;

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_type(
        const size_t port,
        const DataType type) override;

    DataType get_output_type(const size_t port) const override;

    std::unique_ptr<CompiledBlockInterface> get_compiled() const override;

    std::shared_ptr<Model> get_model();

    std::shared_ptr<const Model> get_model() const;

protected:
    std::vector<DataType> input_types;
    std::vector<DataType> output_types;

    std::shared_ptr<Model> model;
};

}

#endif // TF_MODEL_BLOCK_HPP
