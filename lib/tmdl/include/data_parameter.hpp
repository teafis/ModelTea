// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_DATA_PARAMETER_HPP
#define TF_MODEL_DATA_PARAMETER_HPP

#include <memory>

#include "value.hpp"
#include "value_array.hpp"

namespace tmdl {

class DataParameter {
public:
    virtual ~DataParameter() = default;
    virtual void set_from_string(const std::string& s) = 0;
    virtual void set_data_type(DataType dt) = 0;
    virtual void set_data_type_string(const std::string& s, DataType dt) = 0;
    virtual DataType data_type() const = 0;
};

class DataParameterValue : public DataParameter {
public:
    DataParameterValue() = default;

    void set_from_string(const std::string& s) override;

    void set_data_type(const DataType dt) override;

    void set_data_type_string(const std::string& s, DataType dt) override;

private:
    std::shared_ptr<ModelValue> value{ModelValue::make_default(DataType::F64)};
};

class DataParameterArray : public DataParameter {
public:
    DataParameterArray() = default;

    void set_from_string(const std::string& s) override;

    void set_data_type(const DataType dt) override;

    void set_data_type_string(const std::string& s, DataType dt) override;

    void set_size(const size_t c, const size_t r);

private:
    std::shared_ptr<ValueArray> array{ValueArray::create_value_array("[]", DataType::F64)};
};

}

#endif // TF_MODEL_DATA_PARAMETER_HPP
