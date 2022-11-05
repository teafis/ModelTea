// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_HPP
#define TF_MODEL_HPP

#include <memory>
#include <vector>
#include <unordered_map>

#include <tmdl/block.hpp>
#include <tmdl/connection.hpp>

namespace tmdl
{

class Model
{
public:


protected:
    std::vector<size_t> compile_execution_order();

protected:
    std::unordered_map<size_t, std::unique_ptr<Block>> blocks;
    std::vector<Connection> connections;
};

}

#endif // TF_MODEL_HPP
