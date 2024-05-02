// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_MODEL_LIBRARY_HPP
#define TF_MODEL_MODEL_LIBRARY_HPP

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "../library.hpp"
#include "../model.hpp"

namespace tmdl {

class ModelLibrary : public LibraryBase {
protected:
    using model_map_t = std::unordered_map<std::string, std::shared_ptr<Model>>;

public:
    [[nodiscard]] const std::string get_library_name() const override;

    [[nodiscard]] std::vector<std::string> get_block_names() const override;

    [[nodiscard]] bool has_block(std::string_view name) const override;

    [[nodiscard]] std::unique_ptr<BlockInterface> create_block(std::string_view name) const override;

    [[nodiscard]] std::shared_ptr<Model> get_model(std::string_view name) const;

    [[nodiscard]] std::shared_ptr<Model> create_model();

    [[nodiscard]] std::shared_ptr<tmdl::Model> add_model(std::shared_ptr<Model> model);

    void save_model(const Model* model);

    void save_model(const Model* model, const std::filesystem::path& path);

    std::shared_ptr<Model> load_model(const std::filesystem::path& path);

    void close_model(const tmdl::Model* model);

    void close_unused_models();

    [[nodiscard]] std::shared_ptr<Model> try_get_model(std::string_view name) const;

protected:
    [[nodiscard]] model_map_t::iterator find_model(const Model* mdl);
    [[nodiscard]] model_map_t::const_iterator find_model(const Model* mdl) const;

    [[nodiscard]] static bool is_valid_model(const Model* mdl);

private:
    inline static std::string library_name = "models";
    std::unordered_map<std::string, std::shared_ptr<Model>> models;
};

}

#endif // TF_MODEL_MODEL_LIBRARY_HPP
