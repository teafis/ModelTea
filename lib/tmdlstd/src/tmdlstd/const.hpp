// SPDX-License-Identifier: MIT

#ifndef TMDL_STDLIB_CONST_H
#define TMDL_STDLIB_CONST_H

namespace tmdlstd
{

template <typename T>
struct const_block
{
    struct output_t
    {
        T val;
    };

    const_block(const T val) : s_out{ .val = val }
    {
        // Empty Constructor
    }

    const output_t s_out;
};

}

#endif // TMDL_STDLIB_CONST_H
