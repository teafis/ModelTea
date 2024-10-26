// SPDX-License-Identifier: GPL-3.0-only

#ifndef MTEA_DYNDATA_TYPES_H
#define MTEA_DYNDATA_TYPES_H

#include <cstdint>
#include <string>

#include "identifier.hpp"

#include "mtea_types.hpp"

namespace mtea {

template <DataType DT>
using data_type_t = mtea::type_info<DT>;

}

#endif // MTEA_DYNDATA_TYPES_H
