// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VALUE_HPP
#define TF_MODEL_VALUE_HPP

#include <cstdint>

#include <memory>


namespace tmdl
{

enum class DataType
{
    UNKNOWN,
    DATA_TYPE,
    DOUBLE,
    BOOLEAN,
    INT32,
    UINT32
};

struct PortValue
{
    DataType dtype = DataType::UNKNOWN;
    void* ptr = nullptr;

    bool operator==(const PortValue& other) const = default;
};

}

#endif // TF_MODEL_VALUE_HPP
