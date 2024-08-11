// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_DATA_TYPES_H
#define TF_MODEL_DATA_TYPES_H

#include <cstdint>
#include <string>

#include "identifier.hpp"

#include "mtstdlib_types.hpp"

namespace tmdl {

using DataType = mt::stdlib::DataType;

template <DataType DT>
using data_type_t = mt::stdlib::type_info<DT>;

}

#endif // TF_MODEL_DATA_TYPES_H
