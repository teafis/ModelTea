// SPDX-License-Identifier: GPL-3.0-only

#include "model.hpp"

#include <fstream>

#include "blocks/io_ports.hpp"
#include "model_exception.hpp"

#include "model_block.hpp"

#include "codegen/codegen.hpp"
#include "codegen/component.hpp"

#include "library_manager.hpp"

#include "util/identifiers.hpp"

#include <fmt/format.h>


using namespace tmdl;

const std::string tmdl::Model::DEFAULT_MODEL_EXTENSION = ".tmdl";

/* ==================== MODEL LINKS DATA =================== */

struct tmdl::Model::CompiledModelData
{
    struct Links
    {
        struct Block
        {
            size_t block_id;
            size_t port_num;
        };

        std::unordered_map<size_t, std::vector<Block>> input_port_links; // Defines input ports -> block input ports
        std::unordered_map<size_t, Block> output_port_links; // Defines output ports <- block output ports
        std::unordered_map<size_t, std::unordered_map<size_t, std::vector<Block>>> component_links; // Defines block_id -> (output port, block input port)
    };

    Links links;
    std::vector<size_t> execution_order;
};

/* ==================== MODEL COMPONENT =================== */

class ModelCodeComponent : public tmdl::codegen::CodeComponent
{
protected:
    struct ComponentVariable
    {
        std::string name;
        std::unique_ptr<const codegen::CodeComponent> component;
    };

public:
    ModelCodeComponent(
        const std::string& model_name,
        const tmdl::Model::CompiledModelData& exec_data,
        const std::vector<tmdl::DataType>& input_types,
        const std::vector<tmdl::DataType>& output_types,
        std::unordered_map<size_t, std::unique_ptr<const codegen::CodeComponent>>&& components) :
        _model_name(model_name),
        _model_data(exec_data),
        _input_types(input_types),
        _output_types(output_types)
    {
        for (auto& [bid, comp] : components)
        {
            _blocks.emplace(
                bid,
                ComponentVariable {
                    .name = fmt::format("_block_{}", bid),
                    .component = std::move(comp)
            });
        }
        components.clear();

        for (size_t i = 0; i < input_types.size(); ++i)
        {
            _input_names.push_back(fmt::format("value_{}", i));
        }

        for (size_t i = 0; i < output_types.size(); ++i)
        {
            _output_names.push_back(fmt::format("value_{}", i));
        }
    }

    virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_input_type() const override
    {
        return tmdl::codegen::InterfaceDefinition("s_in", _input_names);
    }

    virtual std::optional<const tmdl::codegen::InterfaceDefinition> get_output_type() const override
    {
        return tmdl::codegen::InterfaceDefinition("s_out", _output_names);
    }

    virtual std::string get_name_base() const override
    {
        return _model_name;
    }

    virtual std::optional<std::string> get_function_name(tmdl::codegen::BlockFunction ft) const override
    {
        switch (ft)
        {
        case tmdl::codegen::BlockFunction::INIT:
            return "init";
        case tmdl::codegen::BlockFunction::STEP:
            return "step";
        case tmdl::codegen::BlockFunction::RESET:
            return "reset";
        default:
            return {};
        }
    }

protected:
    virtual std::vector<std::string> write_cpp_code(tmdl::codegen::CodeSection section) const override
    {
        switch (section)
        {
        case tmdl::codegen::CodeSection::DECLARATION:
            return write_cpp_implementation();
        default:
            return {};
        }
    }

    std::vector<std::string> write_cpp_implementation() const
    {
        std::string name_base_upper = get_name_base();
        for (auto& c : name_base_upper)
        {
            c = std::toupper(c);
        }

        const std::string HDR_GUARD = fmt::format("GEN_MDL_BLOCK_{}_GUARD", name_base_upper);

        std::vector<std::string> lines;
        lines.push_back(fmt::format("#ifndef {}", HDR_GUARD));
        lines.push_back(fmt::format("#define {}", HDR_GUARD));
        lines.push_back("");

        std::vector<std::string> include_files;
        for (const auto& kv : _blocks)
        {
            const auto& [varname, comp] = kv.second;

            const auto fcn_name = comp->get_module_name();
            if (fcn_name.empty())
            {
                continue;
            }

            if (std::find(include_files.begin(), include_files.end(), fcn_name) == include_files.end())
            {
                include_files.push_back(fcn_name);
            }
        }

        std::sort(include_files.begin(), include_files.end());

        for (const auto& f : include_files)
        {
            lines.push_back(fmt::format("#include \"{}\"", f));
        }

        lines.push_back("");

        lines.push_back(fmt::format("struct {}", get_name_base()));
        lines.push_back("{");

        lines.push_back("    struct input_t");
        lines.push_back("    {");

        for (size_t i = 0; i < _input_types.size(); ++i)
        {
            const auto dt = _input_types[i];
            lines.push_back(fmt::format("        {} {}{{}};", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, dt), _input_names[i]));
        }

        lines.push_back("    };");
        lines.push_back("");

        lines.push_back("    struct output_t");
        lines.push_back("    {");

        for (size_t i = 0; i < _output_types.size(); ++i)
        {
            const auto dt = _output_types[i];
            lines.push_back(fmt::format("        {} {}{{}};", tmdl::codegen::get_datatype_name(tmdl::codegen::Language::CPP, dt), _output_names[i]));
        }

        lines.push_back("    };");
        lines.push_back("");

        lines.push_back(fmt::format("    {}()", get_name_base()));
        lines.push_back("    {");
        lines.push_back("    }");

        lines.push_back("");

        lines.push_back(fmt::format("    {}(const {}&) = delete;", get_name_base(), get_name_base()));
        lines.push_back(fmt::format("    {}& operator=(const {}&) = delete;", get_name_base(), get_name_base()));

        const std::vector<codegen::BlockFunction> functions = {
            tmdl::codegen::BlockFunction::INIT,
            tmdl::codegen::BlockFunction::RESET,
            tmdl::codegen::BlockFunction::STEP,
        };

        for (const auto fcn : functions)
        {
            lines.push_back("");
            lines.push_back(fmt::format("    void {}()", *get_function_name(fcn)));
            lines.push_back("    {");

            for (const auto& bid : _model_data.execution_order)
            {
                auto it = _blocks.find(bid);
                if (it == _blocks.end())
                {
                    continue;
                }

                lines.push_back(fmt::format("        // Block {}", bid));

                // Copy Input Values
                const auto input_def = get_input_type();
                for (const auto& [port_num, destinations] : _model_data.links.input_port_links)
                {
                    for (const auto& dest : destinations)
                    {
                        if (dest.block_id != bid)
                        {
                            continue;
                        }

                        const auto& blk = _blocks.at(dest.block_id);
                        const auto comp = *blk.component->get_input_type();
                        lines.push_back(fmt::format(
                            "        {}.{}.{} = {}.{};",
                            blk.name,
                            comp.get_name(),
                            comp.get_field(dest.port_num),
                            input_def->get_name(),
                            input_def->get_field(port_num)));
                    }
                }

                for (const auto& [src_index, vals] : _model_data.links.component_links)
                {
                    const auto& src_blk = _blocks.at(src_index);
                    const auto src_comp = *src_blk.component->get_output_type();

                    for (const auto& [src_port, dest_links] : vals)
                    {
                        for (const auto& dest_i : dest_links)
                        {
                            if (dest_i.block_id != bid)
                            {
                                continue;
                            }

                            const auto& dst_blk = _blocks.at(dest_i.block_id);
                            const auto dst_comp = *dst_blk.component->get_input_type();

                            lines.push_back(fmt::format(
                                "        {}.{}.{} = {}.{}.{};",
                                dst_blk.name,
                                dst_comp.get_name(),
                                dst_comp.get_field(dest_i.port_num),
                                src_blk.name,
                                src_comp.get_name(),
                                src_comp.get_field(src_port)));
                        }
                    }
                }

                // Call the resulting function name if present
                const auto& [varname, comp] = it->second;
                const auto fcn_name = comp->get_function_name(fcn);
                if (fcn_name)
                {
                    lines.push_back(fmt::format("        {}.{}();", varname, *fcn_name));
                }
            }

            if (fcn == tmdl::codegen::BlockFunction::STEP || fcn == tmdl::codegen::BlockFunction::RESET || fcn == tmdl::codegen::BlockFunction::INIT)
            {
                lines.push_back("        // Copy Output Values");
                const auto output_def = get_output_type();
                for (const auto& [port_num, src] : _model_data.links.output_port_links)
                {
                    const auto& blk = _blocks.at(src.block_id);
                    const auto comp = *blk.component->get_output_type();
                    lines.push_back(fmt::format("        {}.{} = {}.{}.{};", output_def->get_name(), output_def->get_field(port_num), blk.name, comp.get_name(), comp.get_field(src.port_num)));
                }
            }

            lines.push_back("    }");
        }

        lines.push_back("");
        lines.push_back(fmt::format("    input_t {};", get_input_type()->get_name()));
        lines.push_back(fmt::format("    output_t {};", get_output_type()->get_name()));

        if (_blocks.size() > 0)
        {
            lines.push_back("");
            lines.push_back("private:");
            for (const auto& kv : _blocks)
            {
                const auto& [varname, comp] = kv.second;
                std::string args = "";

                for (const auto& a : comp->constructor_arguments())
                {
                    if (args.empty())
                    {
                        args = a;
                    }
                    else
                    {
                        args = fmt::format("{}, {}", args, a);
                    }
                }

                if (!args.empty())
                {
                    args = fmt::format(" {} ", args);
                }

                lines.push_back(fmt::format("    {} {}{{{}}};", comp->get_type_name(), varname, args));
            }
        }

        lines.push_back("};");

        lines.push_back("");
        lines.push_back(fmt::format("#endif // {}", HDR_GUARD));

        return lines;
    }

protected:
    std::string _model_name;
    tmdl::Model::CompiledModelData _model_data;
    std::vector<tmdl::DataType> _input_types;
    std::vector<std::string> _input_names;
    std::vector<tmdl::DataType> _output_types;
    std::vector<std::string> _output_names;
    std::unordered_map<size_t, ComponentVariable> _blocks;
};

/* ==================== MODEL EXECUTOR ==================== */

class ModelExecutor : public ModelExecutionInterface
{
public:
    ModelExecutor(
        const std::shared_ptr<const VariableManager> variable_manager,
        const std::vector<std::shared_ptr<BlockExecutionInterface>>& blocks) :
        variable_manager(variable_manager),
        blocks(blocks)
    {
        // Empty Constructor?
    }

    void init(const SimState& s) override
    {
        for (const auto& b : blocks)
        {
            b->init(s);
        }
    }

    void step(const SimState& state) override
    {
        for (const auto& b : blocks)
        {
            b->step(state);
        }
    }

    void reset(const SimState& s) override
    {
        for (const auto& b : blocks)
        {
            b->reset(s);
        }
    }

    void close() override
    {
        for (const auto& b : blocks)
        {
            b->close();
        }
    }

    virtual std::vector<std::unique_ptr<codegen::CodeComponent>> get_dependent_components() const
    {
        // Get dependent components here
        throw tmdl::codegen::CodegenError("not supported!!");
    }

    virtual std::unique_ptr<codegen::CodeComponent> get_codegen_component() const
    {
        throw tmdl::codegen::CodegenError("not supported!");
    }

    std::shared_ptr<const VariableManager> get_variable_manager() const override
    {
        return variable_manager;
    }

protected:
    std::shared_ptr<const VariableManager> variable_manager;
    std::vector<std::shared_ptr<BlockExecutionInterface>> blocks;
};

/* ==================== MODEL ==================== */

Model::Model() : name(""), description("user-defined model block"), preferred_dt(0.1)
{
}

void Model::add_block(const std::shared_ptr<BlockInterface> block)
{
    add_block(block, get_next_id());
}

void Model::add_block(const std::shared_ptr<BlockInterface> block, const size_t id)
{
    if (blocks.find(id) != blocks.end())
    {
        throw ModelException(fmt::format("block already contains provided ID {}", id));
    }

    // Check if the block is a model pointer value, and if so, prevent recursive additions
    const auto mdl_ptr = std::dynamic_pointer_cast<ModelBlock>(block);
    if (mdl_ptr)
    {
        if (mdl_ptr->get_name() == get_name() || mdl_ptr->get_model()->contains_model_name(get_name()))
        {
            throw ModelException(fmt::format("cannot have recursive model with name {}", mdl_ptr->get_name()));
        }
    }

    if (const auto ptr = std::dynamic_pointer_cast<InputPort>(block); ptr != nullptr)
    {
        input_ids.push_back(id);
    }
    else if (const auto ptr = std::dynamic_pointer_cast<OutputPort>(block); ptr != nullptr)
    {
        output_ids.push_back(id);
    }

    block->set_id(id);
    blocks.insert({id, block});
}

void Model::remove_block(const size_t id)
{
    // Search for the block in the input values
    auto map_it = blocks.find(id);
    if (map_it == blocks.end())
    {
        throw ModelException(fmt::format("id {} not found in the block map", id));
    }

    blocks.erase(map_it);

    // Remove references in the port vectors
    for (auto* vec : { &input_ids, &output_ids })
    {
        auto it = std::find(
            vec->begin(),
            vec->end(),
            id);
        if (it != vec->end())
        {
            vec->erase(it);
        }
    }

    // Set any inputs that have the current id value
    for (auto& c : connections.get_connections())
    {
        if (c->get_from_id() != id) continue;

        get_block(c->get_to_id())->set_input_type(c->get_to_port(), DataType::UNKNOWN);
    }

    // Remove references to the block ID
    connections.remove_block(id);
}

void Model::add_connection(const std::shared_ptr<Connection> connection)
{
    if (connection == nullptr)
    {
        throw ModelException("cannot add a null connection");
    }

    std::shared_ptr<const BlockInterface> from_block = get_block(connection->get_from_id());
    std::shared_ptr<BlockInterface> to_block = get_block(connection->get_to_id());

    if (connection->get_from_port() < from_block->get_num_outputs() &&
        connection->get_to_port() < to_block->get_num_inputs())
    {
        connections.add_connection(connection);
        to_block->set_input_type(
            connection->get_to_port(),
            from_block->get_output_type(connection->get_from_port()));

        to_block->update_block();
    }
    else
    {
        throw ModelException(fmt::format(
            "from ({} < {}) / to ({} < {}) block and port number mismatch",
            connection->get_from_port(), from_block->get_num_outputs(),
            connection->get_to_port(), to_block->get_num_inputs()));
    }
}

void Model::remove_connection(const size_t to_block, const size_t to_port)
{
    const auto c = connections.get_connection_to(to_block, to_port);

    auto blk = get_block(c->get_to_id());
    if (c->get_to_port() < blk->get_num_inputs())
    {
        blk->set_input_type(c->get_to_port(), DataType::UNKNOWN);
    }

    connections.remove_connection(to_block, to_port);
}

std::string Model::get_name() const
{
    return name;
}

std::string Model::get_description() const
{
    return description;
}

void Model::set_description(const std::string& s)
{
    description = s;
}

double Model::get_preferred_dt() const
{
    return preferred_dt;
}

void Model::set_preferred_dt(const double dt)
{
    if (dt < 1e-6)
    {
        throw ModelException(fmt::format("A preferred dt of {} is too small for the current model", dt));
    }

    preferred_dt = dt;
}

size_t Model::get_num_inputs() const
{
    return input_ids.size();
}

size_t Model::get_num_outputs() const
{
    return output_ids.size();
}

const std::vector<size_t>& Model::get_input_ids() const
{
    return input_ids;
}

const std::vector<size_t>& Model::get_output_ids() const
{
    return output_ids;
}

bool Model::update_block()
{
    bool any_updated = true;
    bool model_updated = false;
    size_t update_count = 0;
    const size_t UPDATE_LIMIT = blocks.size() * 2;

    while (any_updated)
    {
        // Set all to not updated
        any_updated = false;

        // Update any port types
        for (const auto& c : connections.get_connections())
        {
            std::shared_ptr<const BlockInterface> from_blk = get_block(c->get_from_id());
            std::shared_ptr<BlockInterface> to_blk = get_block(c->get_to_id());

            to_blk->set_input_type(
                c->get_to_port(),
                from_blk->get_output_type(c->get_from_port()));
        }

        // Check each port for updates
        for (auto& blk : blocks)
        {
            any_updated |= blk.second->update_block();
        }

        // Mark as updated, and break if the count has been exceeded
        if (any_updated)
        {
            model_updated = true;
        }

        if (update_count > UPDATE_LIMIT)
        {
            throw ModelException("iteration limit exceeded trying to update model block");
        }

        update_count += 1;
    }

    return model_updated;
}

std::unique_ptr<const BlockError> Model::has_error() const
{
    for (const auto& b : blocks)
    {
        auto blk_error = b.second->has_error();
        if (blk_error != nullptr)
        {
            return blk_error;
        }
    }

    return own_error();
}

std::unique_ptr<const BlockError> Model::own_error() const
{
    for (const auto& b : blocks)
    {
        const auto blk = b.second;

        const auto mdl_test = std::dynamic_pointer_cast<ModelBlock>(blk);

        if (mdl_test && mdl_test->get_model()->contains_model_name(get_name()))
        {
            return std::make_unique<BlockError>(BlockError
            {
                .id = 0,
                .message = fmt::format("model {} contains recursive reference to itself", get_name())
            });
        }

        for (size_t i = 0; i < blk->get_num_inputs(); ++i)
        {
            if (!connections.has_connection_to(blk->get_id(), i))
            {
                return std::make_unique<BlockError>(BlockError
                {
                    .id = 0,
                    .message = fmt::format("missing input connection to {} port {}", blk->get_id(), i)
                });
            }
        }
    }

    return nullptr;
}

DataType Model::get_input_datatype(const size_t port) const
{
    if (port < get_num_inputs())
    {
        auto blk = std::dynamic_pointer_cast<InputPort>(get_block(input_ids[port]));
        if (blk == nullptr)
        {
            throw ModelException(fmt::format("invalid block found for input port {}", port));
        }

        return blk->get_output_type(0);
    }
    else
    {
        throw ModelException(fmt::format("model input port count exceeded with {} !< {}", port, get_num_inputs()));
    }
}

DataType Model::get_output_datatype(const size_t port) const
{

    if (port < get_num_outputs())
    {
        auto blk = std::dynamic_pointer_cast<OutputPort>(get_block(output_ids[port]));
        if (blk == nullptr)
        {
            throw ModelException("invalid block found for input port");
        }

        return blk->get_output_value();
    }
    else
    {
        throw ModelException("model input port count exceeded");
    }
}

tmdl::Model::CompiledModelData Model::compile_model() const
{
    // Skip if an error is present
    if (const auto err = has_error(); err != nullptr)
    {
        throw ModelException(err->message);
    }

    CompiledModelData data;

    // Add the input blocks as the first to be executed
    for (const size_t i : input_ids)
    {
        data.execution_order.push_back(i);
    }

    // Create a list of remaining ID values
    std::vector<size_t> remaining_id_values;
    for (const auto& it : blocks)
    {
        const bool is_input = std::find(input_ids.begin(), input_ids.end(), it.first) != input_ids.end();
        const bool is_output = std::find(output_ids.begin(), output_ids.end(), it.first) != output_ids.end();

        if (!is_input && !is_output)
        {
            remaining_id_values.push_back(it.first);
        }
    }

    // Add blocks to the list that have all input ports accounted for
    while (remaining_id_values.size() > 0)
    {
        std::optional<size_t> index;

        for (size_t i = 0; i < remaining_id_values.size(); ++i)
        {
            // Extract identifiers
            const size_t id = remaining_id_values[i];
            std::shared_ptr<const BlockInterface> block = get_block(id);

            // Add the index if there are no input ports
            index = i;

            // Search for each port to see if it is complete
            for (size_t port = 0; port < block->get_num_inputs(); ++port)
            {
                // Skip if the input port is a delayed input, but after need to check
                // connections to ensure that all blocks are connected
                const auto from_conn = connections.get_connection_to(id, port);
                const auto from_block = get_block(from_conn->get_from_id());

                if (from_block->outputs_are_delayed())
                {
                    continue;
                }

                // Grab the port
                const auto conn = connections.get_connection_to(id, port);

                // Check if the from block is already in the execution order
                const auto from_it = std::find(
                    data.execution_order.begin(),
                    data.execution_order.end(),
                    conn->get_from_id());

                if (from_it == data.execution_order.end())
                {
                    index = std::nullopt;
                    break;
                }
            }

            if (index.has_value())
            {
                break;
            }
        }

        if (index.has_value())
        {
            auto it = remaining_id_values.begin() + index.value();
            data.execution_order.push_back(*it);
            remaining_id_values.erase(it);
        }
        else
        {
            throw ModelException("unable to solve - no block found to run execution values...");
        }
    }

    // Add remaining output ID values
    for (const size_t i : output_ids)
    {
        data.execution_order.push_back(i);
    }

    // Add output port types
    for (size_t i = 0; i < output_ids.size(); ++i)
    {
        const auto c = connections.get_connection_to(output_ids[i], 0);

        CompiledModelData::Links::Block block;
        block.block_id = c->get_from_id();
        block.port_num = c->get_from_port();

        data.links.output_port_links[i] = block;
    }

    // Add input port types
    for (size_t i = 0; i < input_ids.size(); ++i)
    {
        for (const auto& c : connections.get_connections())
        {
            if (c->get_from_id() != input_ids[i] || c->get_from_port() != 0)
            {
                continue;
            }

            CompiledModelData::Links::Block blk;
            blk.block_id = c->get_to_id();
            blk.port_num = c->get_to_port();

            data.links.input_port_links[i].push_back(blk);
        }
    }

    for (const auto& c : connections.get_connections())
    {
        if (std::find(input_ids.begin(), input_ids.end(), c->get_from_id()) != input_ids.end())
        {
            continue;
        }
        else if (std::find(output_ids.begin(), output_ids.end(), c->get_to_id()) != output_ids.end())
        {
            continue;
        }

        CompiledModelData::Links::Block blk;
        blk.block_id = c->get_to_id();
        blk.port_num = c->get_to_port();

        data.links.component_links[c->get_from_id()][c->get_from_port()].push_back(blk);
    }

    // Return the result
    return data;
}

std::shared_ptr<ModelExecutionInterface> Model::get_execution_interface(
    const size_t block_id,
    const ConnectionManager& outer_connections,
    const VariableManager& outer_variables) const
{
    // Get the execution order
    const std::vector<size_t> order_values = compile_model().execution_order;

    // Construct the variable list values
    auto variables = std::make_shared<VariableManager>();

    // Add output port types
    for (size_t i = 0; i < output_ids.size(); ++i)
    {
        const auto outer_id = VariableIdentifier {
            .block_id = block_id,
            .output_port_num = i
        };

        const auto ptr_val = outer_variables.get_ptr(outer_id);

        const auto c = connections.get_connection_to(output_ids[i], 0);
        const auto inner_id = VariableIdentifier
        {
            .block_id = c->get_from_id(),
            .output_port_num = c->get_from_port()
        };

        variables->add_variable(inner_id, ptr_val);
    }

    // Add input port types
    for (size_t i = 0; i < input_ids.size(); ++i)
    {
        const auto outer_connection = outer_connections.get_connection_to(block_id, i);
        const auto ptr_val = outer_variables.get_ptr(*outer_connection);

        const auto inner_id = VariableIdentifier
        {
            .block_id = input_ids[i],
            .output_port_num = 0
        };

        variables->add_variable(inner_id, ptr_val);
    }

    // Add interior block types
    for (const auto& bv : blocks)
    {
        // Grab block, but skip if an input or output port, as it would have been updated above
        const auto blk = bv.second;

        for (size_t i = 0; i < blk->get_num_outputs(); ++i)
        {
            const VariableIdentifier vid{
                .block_id = blk->get_id(),
                .output_port_num = i
            };

            // Skip if variable already added (due to input/output)
            if (!variables->has_variable(vid))
            {
                const auto pv = get_block(vid.block_id)->get_output_type(vid.output_port_num);
                variables->add_variable(vid, ModelValue::make_default_type(pv));
            }
        }
    }

    // Construct the interface order value
    std::vector<std::shared_ptr<BlockExecutionInterface>> interface_order;
    for (const auto& b_id : order_values)
    {
        std::shared_ptr<BlockExecutionInterface> block = get_block(b_id)->get_compiled()->get_execution_interface(
            connections,
            *variables);
        interface_order.push_back(block);
    }

    // Create the executor
    auto model_exec = std::make_shared<ModelExecutor>(variables, interface_order);

    // Return result
    return model_exec;
}

std::unique_ptr<codegen::CodeComponent> Model::get_codegen_component() const
{
    // Get the execution order
    const auto exec_data = compile_model();

    // Construct the block parameters
    std::unordered_map<size_t, std::unique_ptr<const codegen::CodeComponent>> components;
    for (const auto& id : exec_data.execution_order)
    {
        if (std::find(input_ids.begin(), input_ids.end(), id) != input_ids.end())
        {
            // Skip Input
        }

        else if (std::find(output_ids.begin(), output_ids.end(), id) != output_ids.end())
        {
            // Skip Output
        }
        else
        {
            const auto blk = get_block(id);
            components.emplace(id, blk->get_compiled()->get_codegen_self());
        }
    }

    std::vector<DataType> input_types;
    std::vector<DataType> output_types;

    for (size_t i = 0; i < input_ids.size(); ++i)
    {
        input_types.push_back(get_input_datatype(i));
    }

    for (size_t i = 0; i < output_ids.size(); ++i)
    {
        output_types.push_back(get_output_datatype(i));
    }

    // Return the results
    return std::make_unique<ModelCodeComponent>(name, exec_data, input_types, output_types, std::move(components));
}

std::vector<std::unique_ptr<codegen::CodeComponent>> Model::get_all_sub_components() const
{
    std::vector<std::unique_ptr<codegen::CodeComponent>> components;

    for (const auto& kv : blocks)
    {
        for (auto& c : kv.second->get_compiled()->get_codegen_components())
        {
            components.push_back(std::move(c));
        }
    }

    return components;
}

std::vector<std::unique_ptr<const BlockError>> Model::get_all_errors() const
{
    std::vector<std::unique_ptr<const BlockError>> error_vals;

    for (const auto& kv : blocks)
    {
        const auto blk = kv.second;
        auto err = blk->has_error();
        if (err != nullptr)
        {
            error_vals.push_back(std::move(err));
        }
    }

    auto oe = own_error();
    if (oe != nullptr)
    {
        error_vals.push_back(std::move(oe));
    }

    return error_vals;
}

const ConnectionManager& Model::get_connection_manager() const
{
    return connections;
}

size_t Model::get_next_id() const
{
    size_t current_id = 0;
    while (blocks.find(current_id) != blocks.end())
    {
        current_id += 1;
    }

    return current_id;
}

std::shared_ptr<BlockInterface> Model::get_block(const size_t id) const
{
    const auto it = blocks.find(id);
    if (it == blocks.end())
    {
        throw ModelException("unable to find block with given id");
    }
    return it->second;
}

std::vector<std::shared_ptr<BlockInterface>> Model::get_blocks() const
{
    std::vector<std::shared_ptr<BlockInterface>> retval;

    for (const auto& b : blocks)
    {
        retval.push_back(b.second);
    }

    return retval;
}

bool Model::contains_model_name(const std::string& n) const
{
    if (n == name)
    {
        return true;
    }

    for (const auto& b : blocks)
    {
        const auto mdl = std::dynamic_pointer_cast<ModelBlock>(b.second);
        if (mdl && mdl->get_name() == n)
        {
            return true;
        }
    }

    return false;
}

void Model::set_filename(const std::filesystem::path& fn)
{
    if (!fn.has_stem())
    {
        throw ModelException(fmt::format("error setting filename '{}' due to missing stem", fn.string()));
    }

    const std::string name_temp = fn.stem();

    if (!is_valid_identifier(name_temp))
    {
        throw ModelException(fmt::format("error setting filename '{}' - '{}' is not a valid identifier", fn.string(), name_temp));
    }

    filename = fn;
    name = name_temp;
}

const std::optional<std::filesystem::path>& Model::get_filename() const
{
    return filename;
}

void Model::clear_filename()
{
    filename.reset();
}

std::shared_ptr<tmdl::Model> tmdl::Model::load_model(const std::filesystem::path& path)
{
    std::ifstream iss(path);
    if (!iss)
    {
        throw ModelException(fmt::format("unable to open file for model with '{}'", path.string()));
    }

    nlohmann::json j;
    iss >> j;

    std::shared_ptr<tmdl::Model> mdl = std::make_shared<tmdl::Model>();
    mdl->set_filename(path);

    tmdl::from_json(j["model"], *mdl);

    return mdl;
}

void tmdl::Model::save_model() const
{
    if (filename.has_value())
    {
        save_model_to(*filename);
    }
    else
    {
        throw ModelException("cannot save model without stored filename");
    }
}

void tmdl::Model::save_model_to(const std::filesystem::path& path) const
{
    std::ofstream oss(path);

    nlohmann::json j;
    j["model"] = *this;

    oss << std::setw(4) << j;
}

struct SaveParameter
{
    std::string id;
    tmdl::ParameterValue::Type dtype;
    std::string value;
};

void to_json(nlohmann::json& j, const SaveParameter& p)
{
    j["id"] = p.id;
    j["dtype"] = p.dtype;
    j["value"] = p.value;
}

void from_json(const nlohmann::json& j, SaveParameter& p)
{
    j.at("id").get_to(p.id);
    j.at("dtype").get_to(p.dtype);
    j.at("value").get_to(p.value);
}

struct SaveBlock
{
    size_t id;
    std::string name;
    std::vector<SaveParameter> parameters;
    int64_t x;
    int64_t y;
};

void to_json(nlohmann::json& j, const SaveBlock& b)
{
    j["id"] = b.id;
    j["name"] = b.name;
    j["parameters"] = b.parameters;
    j["x"] = b.x;
    j["y"] = b.y;
}

void from_json(const nlohmann::json& j, SaveBlock& b)
{
    j.at("id").get_to(b.id);
    j.at("name").get_to(b.name);
    j.at("parameters").get_to(b.parameters);
    j.at("x").get_to(b.x);
    j.at("y").get_to(b.y);
}

void tmdl::to_json(nlohmann::json& j, const tmdl::Model& m)
{
    j["description"] = m.description;
    j["preferred_dt"] = m.preferred_dt;
    j["output_ids"] = m.output_ids;
    j["input_ids"] = m.input_ids;
    j["connections"] = m.connections;

    std::unordered_map<std::string, SaveBlock> json_blocks;
    for (const auto& kv : m.blocks)
    {
        const auto blk = kv.second;

        std::vector<SaveParameter> json_parameters;
        for (const auto& p : blk->get_parameters())
        {
            json_parameters.push_back(SaveParameter
           {
               .id = p->get_id(),
               .dtype = p->get_value().dtype,
               .value = p->get_value().to_string(),
           });
        }

        SaveBlock save_blk
        {
            .id = blk->get_id(),
            .name = blk->get_name(),
            .parameters = json_parameters,
            .x = blk->get_loc().x,
            .y = blk->get_loc().y
        };

        json_blocks.insert({std::to_string(save_blk.id), save_blk});
    }

    j["blocks"] = json_blocks;
}

void tmdl::from_json(const nlohmann::json& j, tmdl::Model& m)
{
    std::string temp_name;

    j.at("description").get_to(m.description);

    if (j.contains("preferred_dt"))
    {
        double dt;
        j.at("preferred_dt").get_to(dt);
        m.set_preferred_dt(dt);
    }

    j.at("output_ids").get_to(m.output_ids);
    j.at("input_ids").get_to(m.input_ids);
    from_json(j.at("connections"), m.connections);

    const auto json_blocks = j.at("blocks").get<std::unordered_map<std::string, SaveBlock>>();

    const auto& lib = tmdl::LibraryManager::get_instance();
    const auto modellib = lib.default_model_library();

    for (const auto& kv : json_blocks)
    {
        const auto& json_blk = kv.second;

        auto blk = lib.try_create_block(json_blk.name);

        if (blk == nullptr)
        {
            auto test_path = m.get_filename();
            if (test_path.has_value())
            {
                auto pth = *test_path;
                pth = pth.parent_path() / std::filesystem::path(json_blk.name).replace_extension(tmdl::Model::DEFAULT_MODEL_EXTENSION);

                modellib->add_model(tmdl::Model::load_model(pth));
                blk = lib.try_create_block(json_blk.name);
            }
        }

        if (blk == nullptr)
        {
            throw tmdl::ModelException(fmt::format("unable to create model due to missing block '{}'", json_blk.name));
        }

        blk->set_id(json_blk.id);
        blk->set_loc(BlockLocation{json_blk.x, json_blk.y});

        for (const auto& prm : json_blk.parameters)
        {
            bool prm_found = false;

            for (const auto& p : blk->get_parameters())
            {
               if (p->get_id() != prm.id) continue;

               p->get_value() = ParameterValue::from_string(prm.value, prm.dtype);

               prm_found = true;
               break;
            }

            if (!prm_found)
            {
                throw tmdl::ModelException("missing parameter id for provided block");
            }
        }

        m.blocks[blk->get_id()] = blk;
    }

    m.update_block();
}
