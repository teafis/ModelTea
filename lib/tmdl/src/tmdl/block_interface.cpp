// SPDX-License-Identifier: GPL-3.0-only

#include "block_interface.hpp"

#include <fmt/format.h>

tmdl::BlockError::BlockError(const size_t id, const std::string& message) : id{id}, message{message} {
    // Empty Constructor
}

tmdl::BlockLocation::BlockLocation() : x{0}, y{0} {
    // Empty Constructor
}

tmdl::BlockLocation::BlockLocation(const int64_t x, const int64_t y) : x{x}, y{y} {
    // Empty Constructor
}

std::vector<std::unique_ptr<tmdl::codegen::CodeComponent>> tmdl::CompiledBlockInterface::get_codegen_components() const {
    std::vector<std::unique_ptr<tmdl::codegen::CodeComponent>> components;

    if (auto self_block = get_codegen_self(); !self_block->is_virtual()) {
        components.push_back(std::move(self_block));
    }

    for (auto& c : get_codegen_other()) {
        if (c->is_virtual()) {
            continue;
        }

        if (std::ranges::find_if(components, [&c](const std::unique_ptr<tmdl::codegen::CodeComponent>& x) {
                return c->get_name_base() == x->get_name_base();
            }) == components.end()) {
            components.push_back(std::move(c));
        }
    }

    return components;
}

std::vector<std::unique_ptr<tmdl::codegen::CodeComponent>> tmdl::CompiledBlockInterface::get_codegen_other() const { return {}; }

tmdl::BlockInterface::ModelInfo::ModelInfo(const double dt) : dt(dt) {
    // Empty Constructor
}

double tmdl::BlockInterface::ModelInfo::get_dt() const { return dt; }

size_t tmdl::BlockInterface::get_id() const { return _id; }

void tmdl::BlockInterface::set_id(const size_t id) { _id = id; }

void tmdl::BlockInterface::set_loc(const BlockLocation& loc) { _loc = loc; }

const tmdl::BlockLocation& tmdl::BlockInterface::get_loc() const { return _loc; }

void tmdl::BlockInterface::set_inverted(bool inverted) { _inverted = inverted; }

bool tmdl::BlockInterface::get_inverted() const { return _inverted; }

std::vector<std::shared_ptr<tmdl::Parameter>> tmdl::BlockInterface::get_parameters() const { return {}; }

bool tmdl::BlockInterface::outputs_are_delayed() const { return false; }

std::unique_ptr<const tmdl::BlockError> tmdl::BlockInterface::make_error(const std::string& msg) const {
    return std::make_unique<BlockError>(get_id(), msg);
}

std::string tmdl::BlockInterface::get_full_name() const { return fmt::format("{}::{}", get_library(), get_name()); }

void tmdl::BlockExecutionInterface::reset() {
    update_inputs();
    blk_reset();
    update_outputs();
}

void tmdl::BlockExecutionInterface::step() {
    update_inputs();
    blk_step();
    update_outputs();
}

void tmdl::BlockExecutionInterface::blk_reset() {
    // Empty Function
}

void tmdl::BlockExecutionInterface::blk_step() {
    // Empty Function
}
