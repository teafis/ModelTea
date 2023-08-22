// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VALUE_ARRAY_HPP
#define TF_MODEL_VALUE_ARRAY_HPP

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "../model_exception.hpp"

#include "value.hpp"

namespace tmdl
{

class ValueArray
{
public:
    virtual void resize(const size_t c, const size_t r) = 0;
    virtual void set_values(const std::vector<std::unique_ptr<const ModelValue>>& values) = 0;
    virtual std::vector<std::unique_ptr<ModelValue>> get_values() const = 0;

public:
    virtual ~ValueArray() = default;

    virtual size_t rows() const = 0;

    virtual size_t cols() const = 0;

    virtual size_t size() const = 0;

    virtual DataType data_type() const = 0;

    virtual std::string to_string() const = 0;

    static std::unique_ptr<ValueArray> create_value_array(const std::string& s, DataType dt);

    static std::unique_ptr<ValueArray> change_array_type(const ValueArray* arr, DataType dt);

    static std::unique_ptr<ValueArray> create_with_type(const size_t cols, const size_t rows, const std::vector<std::unique_ptr<const ModelValue>>& values, const tmdl::DataType data_type);
};

template <DataType DT>
class ValueArrayBox : public ValueArray
{
public:
    using data_t = typename data_type_t<DT>::type;

public:
    struct Index
    {
        size_t col;
        size_t row;
    };

public:
    ValueArrayBox(const size_t c, const size_t r, const std::vector<std::unique_ptr<const ModelValue>>& values = {}) :
        m_data(r * c),
        m_cols{c},
        m_rows{r}
    {
        if (m_data.size() == 0 && (r != 0 || c != 0))
        {
            throw ModelException("2D array cannot have value with size 0");
        }

        set_values(values);
    }

    virtual size_t rows() const override
    {
        return m_rows;
    }

    virtual size_t cols() const override
    {
        return m_cols;
    }

    virtual size_t size() const override
    {
        return m_rows * m_cols;
    }

    virtual DataType data_type() const override
    {
        return DT;
    }

    virtual std::string to_string() const override
    {
        std::ostringstream oss;
        oss << '[';
        for (size_t r = 0; r < m_rows; ++r)
        {
            for (size_t c = 0; c < m_cols; ++c)
            {
                const size_t ind = rc_to_index({c, r});
                oss << std::to_string(m_data[ind]);
                if (c + 1 < m_cols)
                {
                    oss << ", ";
                }
            }
            if (r + 1 < m_rows)
            {
                oss << "; ";
            }
        }
        oss << ']';
        return oss.str();
    }

    virtual void resize(const size_t c, const size_t r) override
    {
        const size_t new_size = r * c;
        if (new_size == 0 && (r != 0 || c != 0))
        {
            throw ModelException("2D array cannot have value with size 0");
        }
        m_data.resize(new_size);
    }

    data_t& operator[](Index i)
    {
        return m_data[rc_to_index(i)];
    }

    const data_t& operator[](Index i) const
    {
        return m_data[rc_to_index(i)];
    }

    virtual void set_values(const std::vector<std::unique_ptr<const ModelValue>>& values) override
    {
        if (values.size() != m_data.size())
        {
            throw ModelException("values has a different size than array");
        }

        for (size_t i = 0; i < values.size(); ++i)
        {
            if (auto v = dynamic_cast<const ModelValueBox<DT>*>(values[i].get()))
            {
                m_data[i] = v->value;
            }
            else
            {
                throw ModelException("cannot set array with mismatching data type");
            }
        }
    }

    virtual std::vector<std::unique_ptr<ModelValue>> get_values() const override
    {
        std::vector<std::unique_ptr<ModelValue>> values;
        for (const auto& v : m_data)
        {
            values.push_back(std::make_unique<ModelValueBox<DT>>(v));
        }

        if (values.size() != m_data.size())
        {
            throw ModelException("mismatch in data sizes for get_values() in model array");
        }

        return values;
    }

private:
    size_t rc_to_index(const Index& rc) const
    {
        return rc.row + rc.col * m_rows;
    }

private:
    std::vector<data_t> m_data;
    size_t m_cols;
    size_t m_rows;
};

}

#endif // TF_MODEL_VALUE_ARRAY_HPP
