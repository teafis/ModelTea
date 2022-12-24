// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_MODEL_LIBRARY_HPP
#define TF_MODEL_MODEL_LIBRARY_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "../library.hpp"
#include "../model.hpp"

namespace tmdl
{

class ModelLibrary : public LibraryBase
{
public:
    std::string get_library_name() const override;

    std::vector<std::string> get_block_names() const override;

    bool has_block(const std::string name) const override;

    std::shared_ptr<BlockInterface> create_block(const std::string& name) const override;

public:
    std::shared_ptr<Model> get_model(const std::string& name) const;

    std::shared_ptr<Model> create_model();

    std::shared_ptr<Model> create_model(const std::string& name);

    void add_model(std::shared_ptr<Model> model);

protected:
    std::shared_ptr<Model> try_get_model(const std::string& name) const;

protected:
    std::vector<std::shared_ptr<Model>> models;
};

}

#endif // TF_MODEL_MODEL_LIBRARY_HPP