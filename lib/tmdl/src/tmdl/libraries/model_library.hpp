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
    [[nodiscard]] const std::string get_library_name() const override;

    [[nodiscard]] std::vector<std::string> get_block_names() const override;

    [[nodiscard]] bool has_block(std::string_view name) const override;

    [[nodiscard]] std::shared_ptr<BlockInterface> create_block(std::string_view name) const override;

    [[nodiscard]] std::shared_ptr<Model> get_model(std::string_view name) const;

    [[nodiscard]] std::shared_ptr<Model> create_model();

    [[nodiscard]] std::shared_ptr<tmdl::Model> add_model(std::shared_ptr<Model> model);

    void close_model(const tmdl::Model* model);

    void close_unused_models();

    [[nodiscard]] std::shared_ptr<Model> try_get_model(std::string_view name) const;

protected:
    [[nodiscard]] static bool is_valid_model(const std::shared_ptr<Model> mdl);

private:
    inline static std::string library_name = "models";
    std::unordered_map<std::string, std::shared_ptr<Model>> models;
};

}

#endif // TF_MODEL_MODEL_LIBRARY_HPP
