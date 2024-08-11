// SPDX-License-Identifier: GPL-3.0-only

#include "library_model.hpp"

#include "model_exception.hpp"
#include "identifier.hpp"

#include <fmt/format.h>

const std::string tmdl::ModelLibrary::get_library_name() const { return library_name; }

std::vector<std::string> tmdl::ModelLibrary::get_block_names() const {
    std::vector<std::string> names;
    for (const auto& m : models | std::views::values) {
        if (m == nullptr) {
            continue;
        } else if (!is_valid_model(m.get())) {
            continue;
        }
        names.push_back(m->get_name());
    }
    return names;
}

bool tmdl::ModelLibrary::has_block(const std::string_view name) const { return try_get_model(name) != nullptr; }

std::unique_ptr<tmdl::BlockInterface> tmdl::ModelLibrary::create_block(const std::string_view name) const {
    return std::make_unique<ModelBlock>(get_model(name), get_library_name());
}

std::unique_ptr<tmdl::BlockInterface> tmdl::ModelLibrary::create_block(tmdl::Model* model) const {
    for (const auto it : models | std::views::values) {
        if (it.get() == model) {
            return std::make_unique<ModelBlock>(it, get_library_name());
        }
    }

    throw tmdl::ModelException(fmt::format("unable to find model matching provided model input in library {}", get_library_name()));
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::get_model(const std::string_view name) const {
    const auto mdl = try_get_model(name);

    if (mdl == nullptr) {
        throw ModelException("no model exists");
    } else if (!is_valid_model(mdl.get())) {
        throw ModelException(fmt::format("model '{}' is not valid", name));
    }

    return mdl;
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::create_new_model() {
    const auto mdl = std::make_shared<tmdl::Model>();
    return add_model(mdl);
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::add_model(std::shared_ptr<Model> model) {
    if (try_get_model(model->get_name()) != nullptr || find_model(model.get()) != models.end()) {
        throw ModelException(fmt::format("cannot add model - model with name '{}' already exists", model->get_name()));
    }

    models.insert({model->get_name(), model});
    return model;
}

void tmdl::ModelLibrary::save_model(const Model* model) {
    if (const auto it = find_model(model); it != models.end()) {
        it->second->save_model();
    } else {
        throw ModelException("model not found in library");
    }
}

void tmdl::ModelLibrary::save_model(const Model* model, const std::filesystem::path& path) {
    if (!path.has_stem()) {
        throw ModelException(fmt::format("error setting filename '{}' due to missing stem", path.string()));
    }

    const auto new_name = Identifier(std::string(path.stem()));

    if (try_get_model(new_name.get())) {
        throw ModelException(fmt::format("library already exists a model with the name '{}'", new_name.get()));
    }

    if (const auto it = find_model(model); it != models.end()) {
        const auto new_it = models.insert({new_name.get(), it->second});

        new_it.first->second->set_filename(path);
        new_it.first->second->save_model();

        models.erase(it);
    } else {
        throw ModelException("model not found in library");
    }
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::load_model(const std::filesystem::path& path) {
    if (!path.has_stem()) {
        throw ModelException(fmt::format("error setting filename '{}' due to missing stem", path.string()));
    }

    const auto new_name = Identifier(std::string(path.stem()));

    if (try_get_model(new_name.get())) {
        throw ModelException(fmt::format("library already exists a model with the name '{}'", new_name.get()));
    }

    const auto mdl = Model::load_model(path);
    models.insert({new_name.get(), mdl});
    return mdl;
}

void tmdl::ModelLibrary::close_model(const tmdl::Model* model) {
    if (model == nullptr) {
        return;
    }

    const auto it = find_model(model);
    if (it == models.end()) {
        throw ModelException(fmt::format("cannot find model '{}' to close", model->get_name()));
    } else if (it->second.get() != model) {
        throw ModelException(fmt::format("model '{}' doesn't match library version of model", model->get_name()));
    } else if (it->second.use_count() > 1) {
        throw ModelException(fmt::format("model '{}' is still in use - cannot close", model->get_name()));
    }

    models.erase(it);
}

void tmdl::ModelLibrary::close_unused_models() {
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

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::try_get_model(const std::string_view name) const {
    const auto it = models.find(std::string(name));
    if (it == models.end()) {
        return nullptr;
    }

    const auto mdl = it->second;
    if (mdl && is_valid_model(mdl.get())) {
        return mdl;
    } else {
        return nullptr;
    }
}

tmdl::ModelLibrary::model_map_t::iterator tmdl::ModelLibrary::find_model(const Model* mdl) {
    return std::find_if(models.begin(), models.end(), [mdl](const model_map_t::value_type& m) { return m.second.get() == mdl; });
}

tmdl::ModelLibrary::model_map_t::const_iterator tmdl::ModelLibrary::find_model(const Model* mdl) const {
    return std::find_if(models.cbegin(), models.cend(), [mdl](const model_map_t::value_type& m) { return m.second.get() == mdl; });
}

bool tmdl::ModelLibrary::is_valid_model(const Model* mdl) {
    if (mdl == nullptr)
        return false;
    const auto name = mdl->get_name();
    return name.size() > 0 && Identifier::is_valid_identifier(name);
}
