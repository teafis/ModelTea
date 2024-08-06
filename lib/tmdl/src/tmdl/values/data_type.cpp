// SPDX-License-Identifier: GPL-3.0-only

module;

#include "mtstdlib_types.hpp"

export module tmdl.values:data_type;

namespace tmdl {

export using DataType = mt::stdlib::DataType;

export template <DataType DT>
using data_type_t = mt::stdlib::type_info<DT>;

}
