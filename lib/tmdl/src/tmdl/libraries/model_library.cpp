// SPDX-License-Identifier: GPL-3.0-only

#include "model_library.hpp"

#include "../model_exception.hpp"
#include "../model_block.hpp"

#include <algorithm>

#include <fmt/format.h>


const std::string tmdl::ModelLibrary::get_library_name() const
{
    return library_name;
}

std::vector<std::string> tmdl::ModelLibrary::get_block_names() const
{
    std::vector<std::string> names;
    for (const auto& m : models | std::views::values)
    {
        if (m == nullptr)
        {
            continue;
        }
        else if (!is_valid_model(m))
        {
            continue;
        }
        names.push_back(m->get_name());
    }
    return names;
}

bool tmdl::ModelLibrary::has_block(const std::string_view name) const
{
    return try_get_model(name) != nullptr;
}

std::shared_ptr<tmdl::BlockInterface> tmdl::ModelLibrary::create_block(const std::string_view name) const
{
    auto m = std::make_shared<ModelBlock>(get_model(name));
    m->update_block();
    return m;
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::get_model(const std::string_view name) const
{
    const auto mdl = try_get_model(name);

    if (mdl == nullptr)
    {
        throw ModelException("no model exists");
    }
    else if (!is_valid_model(mdl))
    {
        throw ModelException(fmt::format("model '{}' is not valid", name));
    }

    return mdl;
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::create_model()
{
    const auto mdl = std::make_shared<tmdl::Model>();
    return add_model(mdl);
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::add_model(std::shared_ptr<Model> model)
{
    if (try_get_model(model->get_name()) != nullptr)
    {
        throw ModelException(fmt::format("cannot add model - model with name '{}' already exists", model->get_name()));
    }

    models.insert({model->get_name(), model});
    return model;
}

void tmdl::ModelLibrary::close_model(const tmdl::Model* model)
{
    if (model == nullptr)
    {
        return;
    }

    const auto it = models.find(model->get_name());
    if (it == models.end()) {
        throw ModelException(fmt::format("cannot find model '{}' to close", model->get_name()));
    } else if (it->second.get() != model) {
        throw ModelException(fmt::format("model '{}' doesn't match library version of model", model->get_name()));
    } else if (it->second.use_count() > 1) {
        throw ModelException(fmt::format("model '{}' is still in use - cannot close", model->get_name()));
    }

    models.erase(it);
}

void tmdl::ModelLibrary::close_unused_models()
{
    std::unordered_map<std::string, std::weak_ptr<tmdl::Model>> weak_models;
    for (const auto& [n, m] : models) {
        weak_models.insert({n, m});
    }

    models.clear();

    for (const auto& [n, mw] : weak_models) {
        const auto m = mw.lock();
        if (m != nullptr) {
            models.insert({n, m});
        }
    }
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::try_get_model(const std::string_view name) const
{
    const auto it = models.find(std::string(name));
    if (it == models.end()) {
        return nullptr;
    }

    const auto mdl = it->second;
    if (mdl && is_valid_model(mdl)) {
        return mdl;
    } else {
        return nullptr;
    }
}

bool tmdl::ModelLibrary::is_valid_model(const std::shared_ptr<Model> mdl)
{
    const auto name = mdl->get_name();
    return name.size() > 0 && Identifier::is_valid_identifier(name);
}
