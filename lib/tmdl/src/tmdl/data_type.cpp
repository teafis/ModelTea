// SPDX-License-Identifier: GPL-3.0-only

module;

#include <string>
#include <type_traits>

#include "mtstdlib_types.hpp"
#include "mtstdlib_string.hpp"

export module tmdl:data_type;

namespace tmdl {
    export enum class DataType : std::underlying_type_t<mt::stdlib::DataType> {
        NONE = 0,
        BOOL,
        U8,
        I8,
        U16,
        I16,
        U32,
        I32,
        U64,
        I64,
        F32,
        F64,
    };

    export template <DataType DT>
    using data_type_t = mt::stdlib::type_info<static_cast<mt::stdlib::DataType>(DT)>;

    export std::string datatype_to_string(DataType dtype) {
        return mt::stdlib::datatype_to_string(static_cast<mt::stdlib::DataType>(dtype));
    }
}
