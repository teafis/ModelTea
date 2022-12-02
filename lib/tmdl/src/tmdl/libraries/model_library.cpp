#include "model_library.hpp"

#include "../model_exception.hpp"
#include "../model_block.hpp"

#include <algorithm>


std::string tmdl::ModelLibrary::get_library_name() const
{
    return "models";
}

std::vector<std::string> tmdl::ModelLibrary::get_block_names() const
{
    std::vector<std::string> names;
    for (const auto& m : models)
    {
        names.push_back(m->get_name());
    }
    return names;
}

bool tmdl::ModelLibrary::has_block(const std::string name) const
{
    return try_get_model(name) != nullptr;
}

std::shared_ptr<tmdl::BlockInterface> tmdl::ModelLibrary::create_block(const std::string& name) const
{
    auto m = std::make_shared<ModelBlock>(get_model(name));
    m->update_block();
    return m;
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::get_model(const std::string& name) const
{
    const auto mdl = try_get_model(name);

    if (mdl == nullptr)
    {
        throw ModelException("no model exists");
    }

    return mdl;
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::create_model(const std::string& name)
{
    if (try_get_model(name) == nullptr)
    {
        auto mdl = std::make_shared<Model>(name);
        models.push_back(mdl);

        return mdl;
    }
    else
    {
        throw ModelException("model with name always exists");
    }
}

std::shared_ptr<tmdl::Model> tmdl::ModelLibrary::try_get_model(const std::string& name) const
{
    const auto it = std::find_if(
        models.begin(),
        models.end(),
        [&name](const std::shared_ptr<const Model> m) {
            return m->get_name() == name;
    });

    if (it != models.end())
    {
        return *it;
    }
    else
    {
        return nullptr;
    }
}
