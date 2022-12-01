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

protected:
    std::shared_ptr<Model> models;
};

}

#endif // TF_MODEL_MODEL_LIBRARY_HPP
