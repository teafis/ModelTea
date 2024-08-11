// SPDX-License-Identifier: GPL-3.0-only

module;

#include <memory>

export module tmdl:data_parameter;

import :value;
import :value_array;

namespace tmdl {

export class DataParameter {
public:
    virtual ~DataParameter() = default;
    virtual void set_from_string(const std::string& s) = 0;
    virtual void set_data_type(DataType dt) = 0;
    virtual void set_data_type_string(const std::string& s, DataType dt) = 0;
    virtual DataType data_type() const = 0;
};

export class DataParameterValue : public DataParameter {
public:
    DataParameterValue() = default;

    void set_from_string(const std::string& s) override;

    void set_data_type(const DataType dt) override;

    void set_data_type_string(const std::string& s, DataType dt) override;

private:
    std::shared_ptr<ModelValue> value{ModelValue::make_default(DataType::F64)};
};

export class DataParameterArray : public DataParameter {
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

void tmdl::DataParameterValue::set_from_string(const std::string& s) {
    const auto v = std::unique_ptr<ModelValue>(ModelValue::from_string(s, value->data_type()));
    value->copy_from(v.get());
}

void tmdl::DataParameterValue::set_data_type(const DataType dt) {
    value = std::shared_ptr<ModelValue>(ModelValue::convert_type(value.get(), dt));
}

void tmdl::DataParameterValue::set_data_type_string(const std::string& s, DataType dt) {
    value = std::shared_ptr<ModelValue>(ModelValue::from_string(s, dt));
}

void tmdl::DataParameterArray::set_from_string(const std::string& s) {
    array = std::shared_ptr<ValueArray>(ValueArray::create_value_array(s, array->data_type()));
}

void tmdl::DataParameterArray::set_data_type(const DataType dt) {
    array = std::shared_ptr<ValueArray>(ValueArray::change_array_type(array.get(), dt));
}

void tmdl::DataParameterArray::set_data_type_string(const std::string& s, DataType dt) {
    array = std::shared_ptr<ValueArray>(ValueArray::create_value_array(s, dt));
}

void tmdl::DataParameterArray::set_size(const size_t c, const size_t r) { array->resize(c, r); }
