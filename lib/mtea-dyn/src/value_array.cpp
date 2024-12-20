// SPDX-License-Identifier: GPL-3.0-only

#include "value_array.hpp"

std::unique_ptr<mtea::ValueArray> mtea::ValueArray::create_value_array(const std::string& s, const DataType dt) {
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

std::unique_ptr<mtea::ValueArray> mtea::ValueArray::change_array_type(const ValueArray* arr, DataType dt) {
    if (arr == nullptr)
        throw ModelException("unexpected nullptr");

    const auto old_values = arr->get_values();
    std::vector<std::unique_ptr<const ModelValue>> model_values;

    for (const auto& ov : old_values) {
        model_values.emplace_back(ModelValue::convert_type(ov.get(), dt));
    }

    return create_with_type(arr->cols(), arr->rows(), model_values, dt);
}

std::unique_ptr<mtea::ValueArray> mtea::ValueArray::create_with_type(const size_t cols, size_t rows,
                                                                     const std::vector<std::unique_ptr<const ModelValue>>& values,
                                                                     const mtea::DataType data_type) {
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
