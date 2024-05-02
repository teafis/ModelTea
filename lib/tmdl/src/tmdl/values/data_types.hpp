// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_DATA_TYPES_H
#define TF_MODEL_DATA_TYPES_H

#include <cstdint>
#include <string>

#include "identifiers.hpp"

#include "mtstd_types.hpp"

namespace tmdl {

using DataType = mt::stdlib::DataType;

template <DataType DT>
using data_type_t = mt::stdlib::type_info<DT>;

// TODO - Class-based data type to provide primitives + identifiers + etc?

std::string data_type_to_string(const DataType dtype);

DataType data_type_from_string(const std::string& s);

}

#endif // TF_MODEL_DATA_TYPES_H
