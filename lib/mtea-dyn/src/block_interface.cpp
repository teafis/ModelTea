// SPDX-License-Identifier: GPL-3.0-only

#include "block_interface.hpp"

#include <fmt/format.h>

mtea::BlockError::BlockError(const size_t id, const std::string& message) : id{id}, message{message} {
    // Empty Constructor
}

mtea::BlockLocation::BlockLocation() : x{0}, y{0} {
    // Empty Constructor
}

mtea::BlockLocation::BlockLocation(const int64_t x, const int64_t y) : x{x}, y{y} {
    // Empty Constructor
}

std::vector<std::unique_ptr<mtea::codegen::CodeComponent>> mtea::CompiledBlockInterface::get_codegen_components() const {
    std::vector<std::unique_ptr<mtea::codegen::CodeComponent>> components;

    if (auto self_block = get_codegen_self(); !self_block->is_virtual()) {
        components.push_back(std::move(self_block));
    }

    for (auto& c : get_codegen_other()) {
        if (c->is_virtual()) {
            continue;
        }

        if (std::ranges::find_if(components, [&c](const std::unique_ptr<mtea::codegen::CodeComponent>& x) {
                return c->get_name_base() == x->get_name_base();
            }) == components.end()) {
            components.push_back(std::move(c));
        }
    }

    return components;
}

std::vector<std::unique_ptr<mtea::codegen::CodeComponent>> mtea::CompiledBlockInterface::get_codegen_other() const { return {}; }

mtea::BlockInterface::ModelInfo::ModelInfo(const double dt) : dt(dt) {
    // Empty Constructor
}

double mtea::BlockInterface::ModelInfo::get_dt() const { return dt; }

size_t mtea::BlockInterface::get_id() const { return _id; }

void mtea::BlockInterface::set_id(const size_t id) { _id = id; }

void mtea::BlockInterface::set_loc(const BlockLocation& loc) { _loc = loc; }

const mtea::BlockLocation& mtea::BlockInterface::get_loc() const { return _loc; }

void mtea::BlockInterface::set_inverted(bool inverted) { _inverted = inverted; }

bool mtea::BlockInterface::get_inverted() const { return _inverted; }

std::vector<std::shared_ptr<mtea::Parameter>> mtea::BlockInterface::get_parameters() const { return {}; }

bool mtea::BlockInterface::outputs_are_delayed() const { return false; }

std::unique_ptr<const mtea::BlockError> mtea::BlockInterface::make_error(const std::string& msg) const {
    return std::make_unique<BlockError>(get_id(), msg);
}

std::string mtea::BlockInterface::get_full_name() const { return fmt::format("{}::{}", get_library(), get_name()); }

void mtea::BlockExecutionInterface::reset() {
    update_inputs();
    blk_reset();
    update_outputs();
}

void mtea::BlockExecutionInterface::step() {
    update_inputs();
    blk_step();
    update_outputs();
}

void mtea::BlockExecutionInterface::blk_reset() {
    // Empty Function
}

void mtea::BlockExecutionInterface::blk_step() {
    // Empty Function
}
