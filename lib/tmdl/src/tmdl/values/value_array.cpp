// SPDX-License-Identifier: GPL-3.0-only

#include "value_array.hpp"

std::shared_ptr<tmdl::ValueArray> tmdl::create_value_array(const std::string& s, const DataType dt)
{
    // Initialize parameters
    size_t rows = 0;
    size_t cols = 0;
    std::vector<std::shared_ptr<const ModelValue>> values{};

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
        values.push_back(tmdl::make_value_from_string(string_value, dt));

        // Set the next parameters
        if (s[next] == ',')
        {
            current_row += 1;
            if (cols == 0)
            {
                rows = current_row;
            }
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
    switch (dt)
    {
    case DataType::BOOLEAN:
        return std::make_shared<ValueArrayBox<DataType::BOOLEAN>>(rows, cols, values);
    case DataType::DOUBLE:
        return std::make_shared<ValueArrayBox<DataType::DOUBLE>>(rows, cols, values);
    case DataType::SINGLE:
        return std::make_shared<ValueArrayBox<DataType::SINGLE>>(rows, cols, values);
    case DataType::INT32:
        return std::make_shared<ValueArrayBox<DataType::INT32>>(rows, cols, values);
    case DataType::UINT32:
        return std::make_shared<ValueArrayBox<DataType::UINT32>>(rows, cols, values);
    default:
        throw ModelException("cannot create a value array from an unknown data type");
    }
}
