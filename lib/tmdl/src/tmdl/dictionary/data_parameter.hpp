// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_DATA_PARAMETER_HPP
#define TF_MODEL_DATA_PARAMETER_HPP

#include <memory>

#include "../values/value.hpp"
#include "../values/value_array.hpp"

namespace tmdl
{

class DataParameter
{
public:
    virtual void set_from_string(const std::string& s) = 0;
    virtual void set_data_type(DataType dt) = 0;
    virtual void set_data_type_string(const std::string& s, DataType dt) = 0;
    virtual DataType data_type() const = 0;
};

class DataParameterValue : public DataParameter
{
public:
    DataParameterValue() : value(ModelValue::make_default_type(DataType::DOUBLE))
    {
        // Empty Constructor
    }

    virtual void set_from_string(const std::string& s) override
    {
        const auto v = std::unique_ptr<ModelValue>(ModelValue::from_string(s, value->data_type()));
        value->copy_value(v.get());
    }

    virtual void set_data_type(const DataType dt) override
    {
        value = std::shared_ptr<ModelValue>(ModelValue::convert_type(value.get(), dt));
    }

    virtual void set_data_type_string(const std::string& s, DataType dt) override
    {
        value = std::shared_ptr<ModelValue>(ModelValue::from_string(s, dt));
    }

private:
    std::shared_ptr<ModelValue> value;
};

class DataParameterArray : public DataParameter
{
public:
    DataParameterArray() : array(ValueArray::create_value_array("[]", DataType::DOUBLE))
    {
        // Empty Constructor
    }

    virtual void set_from_string(const std::string& s) override
    {
        array = std::shared_ptr<ValueArray>(ValueArray::create_value_array(s, array->data_type()));
    }

    virtual void set_data_type(const DataType dt) override
    {
        //value = convert_value_type(value, dt);
    }

    virtual void set_data_type_string(const std::string& s, DataType dt) override
    {
        array = std::shared_ptr<ValueArray>(ValueArray::create_value_array(s, dt));
    }

    void set_size(const size_t c, const size_t r)
    {
        array->resize(c, r);
    }

private:
    std::shared_ptr<ValueArray> array;
};

}

#endif // TF_MODEL_DATA_PARAMETER_HPP
