// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCKS_COMMON_HPP
#define TF_MODEL_BLOCKS_COMMON_HPP

#include "../block_interface.hpp"
#include "../model_exception.hpp"
#include "mtstd_creation.hpp"

namespace tmdl::blocks {

class CommonMtStd : public BlockInterface {
public:
    CommonMtStd(const mt::stdlib::BlockInformation blk_info) : blk_info(blk_info)
    {
        mt::stdlib::DataType base_type;

        if (types_contains(blk_info.types, mt::stdlib::BlockInformation::TypeOptions::FLOAT))
        {
            base_type = mt::stdlib::DataType::F64;
        }
        else if (types_contains(blk_info.types, mt::stdlib::BlockInformation::TypeOptions::INTEGRAL))
        {
            base_type = mt::stdlib::DataType::I32;
        }
        else if (types_contains(blk_info.types, mt::stdlib::BlockInformation::TypeOptions::BOOL))
        {
            base_type = mt::stdlib::DataType::BOOL;
        }
        else
        {
            throw ModelException("unknown block type provided");
        }

        interface = mt::stdlib::create_block(
            blk_info.base_name,
            blk_info.sub_name,
            base_type,
            nullptr
        );
    }

    size_t get_num_inputs() const override
    {
        return interface->get_input_num();
    }

    size_t get_num_outputs() const override
    {
        return interface->get_output_num();
    }

    std::vector<std::shared_ptr<Parameter>> get_parameters() const override
    {
        return {};
    }

    bool update_block() override
    {
        // TODO - Create new interface based on input
        return false;
    }

    std::unique_ptr<const BlockError> has_error() const override;

    void set_input_type(const size_t port, const DataType type) override
    {
        if (interface->get_input_type_settable(port))
        {
            interface = mt::stdlib::create_block(
                blk_info.base_name,
                blk_info.sub_name,
                type,
                nullptr)
        }
    }

    DataType get_output_type(const size_t port) const override
    {
        return interface->get_output_type(port);
    }

private:
    size_t currentPrmPortCount() const;

    static bool types_contains(const mt::stdlib::BlockInformation::TypeOptions a, const mt::stdlib::BlockInformation::TypeOptions b)
    {
        return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != 0;
    }

    std::shared_ptr<Parameter> _prmNumInputPorts;
    std::vector<DataType> _inputTypes;
    DataType _outputPort;
    std::unique_ptr<mt::stdlib::block_interface> interface;
    mt::stdlib::BlockInformation blk_info;
};

}

#endif // TF_MODEL_BLOCKS_COMMON_HPP
