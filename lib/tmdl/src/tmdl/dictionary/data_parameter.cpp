// SPDX-License-Identifier: GPL-3.0-only

#include "data_parameter.hpp"

tmdl::DataParameterValue::DataParameterValue() : value(ModelValue::make_default(DataType::DOUBLE))
{
    // Empty Constructor
}

void tmdl::DataParameterValue::set_from_string(const std::string& s)
{
    const auto v = std::unique_ptr<ModelValue>(ModelValue::from_string(s, value->data_type()));
    value->copy_from(v.get());
}

void tmdl::DataParameterValue::set_data_type(const DataType dt)
{
    value = std::shared_ptr<ModelValue>(ModelValue::convert_type(value.get(), dt));
}

void tmdl::DataParameterValue::set_data_type_string(const std::string& s, DataType dt)
{
    value = std::shared_ptr<ModelValue>(ModelValue::from_string(s, dt));
}

tmdl::DataParameterArray::DataParameterArray() : array(ValueArray::create_value_array("[]", DataType::DOUBLE))
{
    // Empty Constructor
}

void tmdl::DataParameterArray::set_from_string(const std::string& s)
{
    array = std::shared_ptr<ValueArray>(ValueArray::create_value_array(s, array->data_type()));
}

void tmdl::DataParameterArray::set_data_type(const DataType dt)
{
    array = std::shared_ptr<ValueArray>(array->change_array_type(array.get(), dt));
}

void tmdl::DataParameterArray::set_data_type_string(const std::string& s, DataType dt)
{
    array = std::shared_ptr<ValueArray>(ValueArray::create_value_array(s, dt));
}

void tmdl::DataParameterArray::set_size(const size_t c, const size_t r)
{
    array->resize(c, r);
}
