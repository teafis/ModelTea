// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_VALUE_ARRAY_HPP
#define TF_MODEL_VALUE_ARRAY_HPP

#include <memory>
#include <string>
#include <vector>

#include "../model_exception.hpp"

#include "value.hpp"

namespace tmdl
{

class ValueArray
{
public:
    virtual DataType data_type() const = 0;
};

template <DataType DT>
class ValueArrayBox : public ValueArray
{
public:
    using data_t = data_type_t<DT>::type;

public:
    struct Index
    {
        size_t row;
        size_t col;
    };

public:
    ValueArrayBox(const size_t r, const size_t c) :
        m_rows{r},
        m_cols{c},
        m_data(r * c)
    {
        if (m_data.size() == 0)
        {
            throw ModelException("2D array cannot have value with size 0");
        }
    }

    ValueArrayBox(const size_t r, const size_t c, const std::vector<std::shared_ptr<const ModelValue>>& values) :
        m_rows{r},
        m_cols{c},
        m_data(r * c)
    {
        if (m_data.size() == 0)
        {
            throw ModelException("2D array cannot have value with size 0");
        }

        set_values(values);
    }

    virtual DataType data_type() const override
    {
        return DT;
    }

    void resize(const size_t r, const size_t c)
    {
        const size_t new_size = r * c;
        if (new_size == 0)
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

    void set_values(const std::vector<std::shared_ptr<const ModelValue>>& values)
    {
        if (values.size() != m_data.size())
        {
            throw ModelException("values has a different size than array");
        }

        for (size_t i = 0; i < values.size(); ++i)
        {
            if (auto v = std::dynamic_pointer_cast<const ModelValueBox<DT>>(values[i]))
            {
                m_data[i] = v->value;
            }
            else
            {
                throw ModelException("cannot set array with mismatching data type");
            }
        }
    }

private:
    size_t rc_to_index(const Index& rc)
    {
        return rc.row + rc.col * m_rows;
    }

private:
    std::vector<data_t> m_data;
    size_t m_rows;
    size_t m_cols;
};

std::shared_ptr<ValueArray> create_value_array(const std::string& s, DataType dt);

}

#endif // TF_MODEL_VALUE_ARRAY_HPP
