#include "model_library.hpp"

#include "../model_exception.hpp"


std::string tmdl::ModelLibrary::get_library_name() const
{
    return "models";
}

std::vector<std::string> tmdl::ModelLibrary::get_block_names() const
{
    return {};
}

bool tmdl::ModelLibrary::has_block(const std::string name) const
{
    (void)name;
    return false;
}

std::shared_ptr<tmdl::BlockInterface> tmdl::ModelLibrary::create_block(const std::string& name) const
{
    (void)name;
    return nullptr;
}
