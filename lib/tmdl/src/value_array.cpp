// SPDX-License-Identifier: GPL-3.0-only

module;

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

export module tmdl:value_array;

import :value;

namespace tmdl {

export class ValueArray {
public:
    virtual void resize(const size_t c, const size_t r) = 0;

    virtual void set_values(const std::vector<std::unique_ptr<const ModelValue>>& values) = 0;

    virtual std::vector<std::unique_ptr<ModelValue>> get_values() const = 0;

    virtual ~ValueArray() = default;

    virtual size_t rows() const = 0;

    virtual size_t cols() const = 0;

    virtual size_t size() const {
        return rows() * cols();
    }

    virtual DataType data_type() const = 0;

    virtual std::string to_string() const = 0;

    static std::unique_ptr<ValueArray> create_value_array(const std::string& s, DataType dt);

    static std::unique_ptr<ValueArray> change_array_type(const ValueArray* arr, DataType dt);

    static std::unique_ptr<ValueArray> create_with_type(const size_t cols, const size_t rows,
                                                        const std::vector<std::unique_ptr<const ModelValue>>& values,
                                                        const tmdl::DataType data_type);
};

export template <DataType DT> class ValueArrayBox : public ValueArray {
public:
    using data_t = typename data_type_t<DT>::type_t;

    struct Index {
        size_t col;
        size_t row;
    };

    ValueArrayBox(const size_t c, const size_t r, const std::vector<std::unique_ptr<const ModelValue>>& values = {})
        : m_data(r * c), m_cols{c}, m_rows{r} {
        if (m_data.size() == 0 && (r != 0 || c != 0)) {
            throw ModelException("2D array cannot have value with size 0");
        }

        set_values(values);
    }

    size_t rows() const override { return m_rows; }

    size_t cols() const override { return m_cols; }

    DataType data_type() const override { return DT; }

    std::string to_string() const override {
        std::ostringstream oss;
        oss << '[';
        for (size_t r = 0; r < m_rows; ++r) {
            for (size_t c = 0; c < m_cols; ++c) {
                const size_t ind = rc_to_index({c, r});
                oss << std::to_string(m_data[ind]);
                if (c + 1 < m_cols) {
                    oss << ", ";
                }
            }
            if (r + 1 < m_rows) {
                oss << "; ";
            }
        }
        oss << ']';
        return oss.str();
    }

    void resize(const size_t c, const size_t r) override {
        const size_t new_size = r * c;
        if (new_size == 0 && (r != 0 || c != 0)) {
            throw ModelException("2D array cannot have value with size 0");
        }
        m_data.resize(new_size);
    }

    data_t& operator[](Index i) { return m_data[rc_to_index(i)]; }

    const data_t& operator[](Index i) const { return m_data[rc_to_index(i)]; }

    void set_values(const std::vector<std::unique_ptr<const ModelValue>>& values) override {
        if (values.size() != m_data.size()) {
            throw ModelException("values has a different size than array");
        }

        for (size_t i = 0; i < values.size(); ++i) {
            if (auto v = dynamic_cast<const ModelValueBox<DT>*>(values[i].get())) {
                m_data[i] = v->value;
            } else {
                throw ModelException("cannot set array with mismatching data type");
            }
        }
    }

    std::vector<std::unique_ptr<ModelValue>> get_values() const override {
        std::vector<std::unique_ptr<ModelValue>> values;
        for (const auto& v : m_data) {
            values.push_back(std::make_unique<ModelValueBox<DT>>(v));
        }

        if (values.size() != m_data.size()) {
            throw ModelException("mismatch in data sizes for get_values() in model array");
        }

        return values;
    }

private:
    size_t rc_to_index(const Index& rc) const { return rc.row + rc.col * m_rows; }

    std::vector<data_t> m_data;
    size_t m_cols;
    size_t m_rows;
};

export template <> class ValueArrayBox<DataType::NONE> : public ValueArray {
public:
    ValueArrayBox([[maybe_unused]] const size_t c, [[maybe_unused]] const size_t r, const std::vector<std::unique_ptr<const ModelValue>>& values = {}) {
        if (values.size() > 0) {
            throw ModelException("cannot set size of a NONE array");
        }
    }

    void resize(const size_t c, const size_t r) override {
        throw ModelException("cannot resize a none array");
    }

    void set_values(const std::vector<std::unique_ptr<const ModelValue>>& values) override {
        throw ModelException("cannot set values to a none array");
    }

    std::vector<std::unique_ptr<ModelValue>> get_values() const override {
        return {};
    }

    size_t rows() const override {
        return 0;
    }

    size_t cols() const override {
        return 0;
    }

    DataType data_type() const override {
        return DataType::NONE;
    }

    std::string to_string() const override {
        return "NONE";
    }
};

}

std::unique_ptr<tmdl::ValueArray> tmdl::ValueArray::create_value_array(const std::string& s, const DataType dt) {
    // Initialize parameters
    size_t rows = 0;
    size_t cols = 0;
    std::vector<std::unique_ptr<const ModelValue>> values{};

    // Find the first index of the array
    const size_t start_index = s.find_first_of('[');
    if (start_index == std::string::npos) {
        throw ModelException("unable to find first bracket");
    }

    bool found_end = false;

    size_t current = start_index + 1;
    size_t next = current;
    size_t current_row = 0;
    while ((next = s.find_first_of(";,]", next)) != std::string::npos && !found_end) {
        // Extract the value
        std::string string_value = s.substr(current, next - current);

        // Trim the start and end value spaces
        if (const size_t t_start = string_value.find_first_not_of(' '); t_start != std::string::npos) {
            string_value = string_value.substr(t_start);
        }

        if (const size_t t_end = string_value.find_first_of(' '); t_end != std::string::npos) {
            string_value = string_value.substr(0, t_end);
        }

        // Set the value
        values.push_back(std::unique_ptr<ModelValue>(ModelValue::from_string(string_value, dt)));

        // Set the next parameters
        if (s[next] == ',') {
            current_row += 1;
            if (cols == 0) {
                rows = current_row;
            }
        } else if (s[next] == ']' && rows == 0 && cols == 0) {
            found_end = true;
        } else if (s[next] == ';' || s[next] == ']') {
            current_row += 1;
            if (cols == 0) {
                rows = current_row;
            } else if (current_row != rows) {
                throw ModelException("each row must have the same values");
            }

            cols += 1;
            current_row = 0;

            if (s[next] == ']') {
                found_end = true;
            }
        } else {
            throw ModelException("unknown array character found");
        }

        // Update output
        if (!found_end) {
            next = current + 1;
        }
    }

    if (!found_end) {
        throw ModelException("unable to find the ending bracket");
    } else if (rows * cols != values.size()) {
        throw ModelException("mismatch in rows/cols and output array size!");
    }

    // Create the array and set values
    return create_with_type(cols, rows, values, dt);
}

std::unique_ptr<tmdl::ValueArray> tmdl::ValueArray::change_array_type(const ValueArray* arr, DataType dt) {
    if (arr == nullptr)
        throw ModelException("unexpected nullptr");

    const auto old_values = arr->get_values();
    std::vector<std::unique_ptr<const ModelValue>> model_values;

    for (const auto& ov : old_values) {
        model_values.emplace_back(ModelValue::convert_type(ov.get(), dt));
    }

    return create_with_type(arr->cols(), arr->rows(), model_values, dt);
}

std::unique_ptr<tmdl::ValueArray> tmdl::ValueArray::create_with_type(const size_t cols, size_t rows,
                                                                     const std::vector<std::unique_ptr<const ModelValue>>& values,
                                                                     const tmdl::DataType data_type) {
    switch (data_type) {
        using enum DataType;
    case BOOL:
        return std::make_unique<ValueArrayBox<BOOL>>(cols, rows, values);
    case F64:
        return std::make_unique<ValueArrayBox<F64>>(cols, rows, values);
    case F32:
        return std::make_unique<ValueArrayBox<F32>>(cols, rows, values);
    case I8:
        return std::make_unique<ValueArrayBox<I8>>(cols, rows, values);
    case U8:
        return std::make_unique<ValueArrayBox<U8>>(cols, rows, values);
    case I16:
        return std::make_unique<ValueArrayBox<I16>>(cols, rows, values);
    case U16:
        return std::make_unique<ValueArrayBox<U16>>(cols, rows, values);
    case I32:
        return std::make_unique<ValueArrayBox<I32>>(cols, rows, values);
    case U32:
        return std::make_unique<ValueArrayBox<U32>>(cols, rows, values);
    case I64:
        return std::make_unique<ValueArrayBox<I64>>(cols, rows, values);
    case U64:
        return std::make_unique<ValueArrayBox<U64>>(cols, rows, values);
    case NONE:
        return std::make_unique<ValueArrayBox<NONE>>(cols, rows, values);
    default:
        throw ModelException("cannot create a value array from an unknown data type");
    }
}
