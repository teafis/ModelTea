// SPDX-License-Identifier: GPL-3.0-only

#include "model_library.hpp"

#include "../model_exception.hpp"
#include "../model_block.hpp"

#include <algorithm>

#include <fmt/format.h>


const std::string& tmdl::ModelLibrary::get_library_name() const
{
    return library_name;
}

std::vector<std::string> tmdl::ModelLibrary::get_block_names() const
{
    std::vector<std::string> names;
    for (const auto& m : models)
    {
        if (!is_valid_model(m))
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
    models.push_back(mdl);
    return mdl;
}

void tmdl::ModelLibrary::add_model(std::shared_ptr<Model> model)
{
    if (try_get_model(model->get_name()) != nullptr)
    {
        throw ModelException(fmt::format("cannot add model - model with name '{}' already exists", model->get_name()));
    }

    models.push_back(model);
}

void tmdl::ModelLibrary::close_model(const tmdl::Model* model)
{
    if (model == nullptr)
    {
        return;
    }

    const auto it = std::ranges::find_if(
        models,
        [&model](const std::shared_ptr<const Model> m) {
            return m.get() == model;
    });

    if (it == models.end())
    {
        throw ModelException(fmt::format("cannot find model '{}' to close", model->get_name()));
    }
    else if (it->use_count() > 1)
    {
        throw ModelException(fmt::format("model '{}' is still in use - cannot close", model->get_name()));
    }

    models.erase(it);
}

void tmdl::ModelLibrary::close_unused_models()
{
    const size_t iter_count = models.size();
    bool any_closed = true;

    for (size_t i = 0; i < iter_count && any_closed; ++i)
    {
        any_closed = false;

        auto it = models.begin();
        while (it != models.end())
        {
            if (it->use_count() > 1)
            {
                ++it;
            }
            else
            {
                it = models.erase(it);
                any_closed = true;
            }
        }
    }
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::try_get_model(const std::string_view name) const
{
    const auto it = std::ranges::find_if(
        models,
        [&name](const std::shared_ptr<const Model> m) {
            return m->get_name() == name;
    });

    if (it != models.end() && is_valid_model(*it))
    {
        return *it;
    }
    else
    {
        return nullptr;
    }
}

bool tmdl::ModelLibrary::is_valid_model(const std::shared_ptr<Model> mdl)
{
    return mdl->get_name().size() > 0;
}
