// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_DATA_PARAMETER_HPP
#define TF_MODEL_DATA_PARAMETER_HPP

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


private:
    std::shared_ptr<ModelValue> value;
};

class DataParameterArray : public DataParameter
{
private:
    std::shared_ptr<ValueArray> array;
};

}

#endif // TF_MODEL_DATA_PARAMETER_HPP
