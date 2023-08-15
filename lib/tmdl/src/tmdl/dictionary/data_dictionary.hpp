// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_DATA_DICTIONARY_HPP
#define TF_MODEL_DATA_DICTIONARY_HPP

#include <unordered_map>

#include "data_parameter.hpp"

namespace tmdl
{

class DataDictionary
{
private:
    std::unordered_map<std::string, DataParameter> vals;
};

}

#endif // TF_MODEL_DATA_DICTIONARY
