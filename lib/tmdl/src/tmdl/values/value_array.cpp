// SPDX-License-Identifier: GPL-3.0-only

#include "value_array.hpp"

tmdl::ValueArray* tmdl::ValueArray::create_value_array(const std::string& s, const DataType dt)
{
    // Initialize parameters
    size_t rows = 0;
    size_t cols = 0;
    std::vector<std::unique_ptr<const ModelValue>> values{};

    // Find the first index of the array
    const size_t start_index = s.find_first_of('[');
    if (start_index == std::string::npos)
    {
        throw ModelException("unable to find first bracket");
    }

    bool found_end = false;

    size_t current = start_index + 1;
    size_t next = current;
    size_t current_row = 0;
    while ((next = s.find_first_of(";,]", next)) != std::string::npos)
    {
        // Extract the value
        std::string string_value = s.substr(current, next - current);

        // Trim the start and end value spaces
        const size_t t_start = string_value.find_first_not_of(' ');
        if (t_start != std::string::npos) string_value = string_value.substr(t_start);

        const size_t t_end = string_value.find_first_of(' ');
        if (t_end != std::string::npos) string_value = string_value.substr(0, t_end);

        // Set the value
        values.push_back(std::unique_ptr<ModelValue>(ModelValue::from_string(string_value, dt)));

        // Set the next parameters
        if (s[next] == ',')
        {
            current_row += 1;
            if (cols == 0)
            {
                rows = current_row;
            }
        }
        else if (s[next] == ']' && rows == 0 && cols == 0)
        {
            found_end = true;
            break;
        }
        else if (s[next] == ';' || s[next] == ']')
        {
            current_row += 1;
            if (cols == 0)
            {
                rows = current_row;
            }
            else if (current_row != rows)
            {
                throw ModelException("each row must have the same values");
            }

            cols += 1;
            current_row = 0;

            if (s[next] == ']')
            {
                found_end = true;
                break;
            }
        }
        else
        {
            throw ModelException("unknown array character found");
        }

        // Update output
        next = current + 1;
    }

    if (!found_end)
    {
        throw ModelException("unable to find the ending bracket");
    }
    else if (rows * cols != values.size())
    {
        throw ModelException("mismatch in rows/cols and output array size!");
    }

    // Create the array and set values
    return create_with_type(cols, rows, values, dt);
}

tmdl::ValueArray* tmdl::ValueArray::change_array_type(const ValueArray* arr, DataType dt)
{
    if (arr == nullptr) throw ModelException("unexpected nullptr");

    const auto old_values = arr->get_values();
    std::vector<std::unique_ptr<const ModelValue>> model_values;

    for (const auto& ov : old_values)
    {
        model_values.emplace_back(ModelValue::convert_type(ov.get(), dt));
    }

    return create_with_type(arr->cols(), arr->rows(), model_values, dt);
}

tmdl::ValueArray* tmdl::ValueArray::create_with_type(const size_t cols, size_t rows, const std::vector<std::unique_ptr<const ModelValue>>& values, const tmdl::DataType data_type)
{
    switch (data_type)
    {
    case DataType::BOOLEAN:
        return new ValueArrayBox<DataType::BOOLEAN>(cols, rows, values);
    case DataType::DOUBLE:
        return new ValueArrayBox<DataType::DOUBLE>(cols, rows, values);
    case DataType::SINGLE:
        return new ValueArrayBox<DataType::SINGLE>(cols, rows, values);
    case DataType::INT32:
        return new ValueArrayBox<DataType::INT32>(cols, rows, values);
    case DataType::UINT32:
        return new ValueArrayBox<DataType::UINT32>(cols, rows, values);
    default:
        throw ModelException("cannot create a value array from an unknown data type");
    }
}
