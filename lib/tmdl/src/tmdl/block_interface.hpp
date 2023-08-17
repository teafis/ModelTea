// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_BLOCK_INTERFACE_HPP
#define TF_MODEL_BLOCK_INTERFACE_HPP

#include <cstdlib>

#include <memory>
#include <string>
#include <vector>

#include "parameter.hpp"

#include "connection_manager.hpp"
#include "variable_manager.hpp"

#include "codegen/component.hpp"


namespace tmdl
{

class BlockExecutionInterface;

struct BlockError
{
    size_t id;
    std::string message;
};

struct BlockLocation
{
    int64_t x;
    int64_t y;

    explicit BlockLocation();

    explicit BlockLocation(const int64_t x, const int64_t y);
};

class CompiledBlockInterface
{
public:
    virtual ~CompiledBlockInterface();

    virtual std::shared_ptr<BlockExecutionInterface> get_execution_interface(
        const ConnectionManager& connections,
        const VariableManager& manager) const = 0;

    std::vector<std::unique_ptr<codegen::CodeComponent>> get_codegen_components() const;

    virtual std::unique_ptr<codegen::CodeComponent> get_codegen_self() const = 0;

protected:
    virtual std::vector<std::unique_ptr<codegen::CodeComponent>> get_codegen_other() const;
};

class BlockInterface
{
public:
    class ModelInfo
    {
    public:
        ModelInfo(const double dt);

        double get_dt() const;

    private:
        const double dt;
    };

public:
    explicit BlockInterface();

    virtual ~BlockInterface();

    size_t get_id() const;

    void set_id(const size_t id);

    void set_loc(const BlockLocation& loc);

    const BlockLocation& get_loc() const;

    virtual std::string get_name() const = 0;

    virtual std::string get_description() const = 0;

    virtual std::vector<std::shared_ptr<Parameter>> get_parameters() const;

    virtual bool update_block() = 0;

    virtual std::unique_ptr<const BlockError> has_error() const = 0; // Return null if no error

    virtual size_t get_num_inputs() const = 0;

    virtual size_t get_num_outputs() const = 0;

    virtual bool outputs_are_delayed() const;

    virtual void set_input_type(
        const size_t port,
        const DataType type) = 0;

    virtual DataType get_output_type(const size_t port) const = 0;

    virtual std::unique_ptr<CompiledBlockInterface> get_compiled(const ModelInfo& s) const = 0;

    BlockInterface& operator=(const BlockInterface&) = delete;

protected:
    std::unique_ptr<const BlockError> make_error(const std::string& msg) const;

protected:
    size_t _id;
    BlockLocation _loc;
};

class BlockExecutionInterface
{
public:
    virtual ~BlockExecutionInterface();

    virtual void init();

    virtual void step();

    virtual void reset();

    virtual void close();
};

}

#endif // TF_MODEL_BLOCK_INTERFACE_HPP
