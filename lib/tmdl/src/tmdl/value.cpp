// SPDX-License-Identifier: GPL-3.0-only

#include <tmdl/value.hpp>
#include <tmdl/model_exception.hpp>

using namespace tmdl;

void tmdl::PortValue::clear()
{
    dtype = DataType::UNKNOWN;
}
