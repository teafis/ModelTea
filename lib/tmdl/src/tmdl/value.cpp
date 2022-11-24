// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/value.hpp>
#include <tmdl/model_exception.hpp>

using namespace tmdl;

void tmdl::PortValue::clear()
{
    dtype = DataType::UNKNOWN;
}

template <typename T>
static std::shared_ptr<tmdl::ValueBox> get_default_shared()
{
    return std::make_shared<ValueBoxType<T>>(T{});
}

std::shared_ptr<tmdl::ValueBox> tmdl::make_shared_default_value(const DataType dtype)
{
    switch (dtype)
    {
    case DataType::BOOLEAN:
        return get_default_shared<bool>();
    case DataType::SINGLE:
        return get_default_shared<float>();
    case DataType::DOUBLE:
        return get_default_shared<double>();
    case DataType::INT32:
        return get_default_shared<int32_t>();
    case DataType::UINT32:
        return get_default_shared<uint32_t>();
    default:
        throw ModelException("unable to construct value for type");
    }
}
