// SPDX-License-Identifier: GPL-3.0-only

module;

#include <cstdlib>

#include <memory>
#include <string>
#include <vector>

export module tmdl:block_interface;

import tmdl.values;

import :codegen_component;
import :variable_manager;

namespace tmdl {

class BlockExecutionInterface;

struct BlockError {
    BlockError(const size_t id, const std::string& message);

    const size_t id;
    const std::string message;
};

struct BlockLocation {
    int64_t x;
    int64_t y;

    explicit BlockLocation();

    explicit BlockLocation(const int64_t x, const int64_t y);
};

class CompiledBlockInterface {
public:
    virtual ~CompiledBlockInterface() = default;

    virtual std::unique_ptr<BlockExecutionInterface> get_execution_interface(const ConnectionManager& connections,
                                                                             const VariableManager& manager) const = 0;

    std::vector<std::unique_ptr<codegen::CodeComponent>> get_codegen_components() const;

    virtual std::unique_ptr<codegen::CodeComponent> get_codegen_self() const = 0;

protected:
    virtual std::vector<std::unique_ptr<codegen::CodeComponent>> get_codegen_other() const;
};

class BlockInterface {
public:
    class ModelInfo {
    public:
        explicit ModelInfo(const double dt);

        double get_dt() const;

    private:
        const double dt;
    };

    BlockInterface(std::string_view lib) : library_name(lib) {}

    virtual ~BlockInterface() = default;

    BlockInterface& operator=(const BlockInterface&) = delete;

    size_t get_id() const;

    void set_id(const size_t id);

    void set_loc(const BlockLocation& loc);

    const BlockLocation& get_loc() const;

    void set_inverted(bool);

    bool get_inverted() const;

    virtual std::string get_name() const = 0;

    virtual std::string get_description() const = 0;

    virtual std::vector<std::shared_ptr<Parameter>> get_parameters() const;

    virtual bool update_block() = 0;

    virtual std::unique_ptr<const BlockError> has_error() const = 0; // Return null if no error

    virtual size_t get_num_inputs() const = 0;

    virtual size_t get_num_outputs() const = 0;

    virtual bool outputs_are_delayed() const;

    virtual void set_input_type(const size_t port, const DataType type) = 0;

    virtual DataType get_output_type(const size_t port) const = 0;

    virtual std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo& s) const = 0;

    virtual std::string get_library() const { return library_name; }

    virtual std::string get_full_name() const;

protected:
    std::unique_ptr<const BlockError> make_error(const std::string& msg) const;

private:
    size_t _id{0};
    BlockLocation _loc{};
    bool _inverted{false};
    const std::string library_name;
};

class BlockExecutionInterface {
public:
    virtual ~BlockExecutionInterface() = default;

    void reset();
    void step();

protected:
    virtual void update_inputs() = 0;
    virtual void update_outputs() = 0;

    virtual void blk_reset();
    virtual void blk_step();
};

}

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
