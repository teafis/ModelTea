// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_PARAMETER_HPP
#define TF_MODEL_PARAMETER_HPP

#include <string>

#include <tmdl/value.hpp>


namespace tmdl
{

class Parameter
{
public:
    Parameter(
        const std::string& id,
        const std::string& name,
        const DataType data_type,
        const std::string& value) :
        id(id),
        name(name),
        value(value),
        data_type(data_type)
    {
        // Empty Constructor
    }

    std::string get_id() const
    {
        return id;
    }

    std::string get_name() const
    {
        return name;
    }

    DataType get_data_type() const
    {
        return data_type;
    }

    void set_data_type(const DataType new_type)
    {
        data_type = new_type;
    }

    void set_value(const std::string& new_value)
    {
        value = new_value;
    }

    std::unique_ptr<Value> get_current_value(const std::string& s) const
    {
        try
        {
            switch (data_type)
            {
            case DataType::BOOLEAN:
                return std::make_unique<BooleanValue>(std::stoi(s) != 0);
            case DataType::INT32:
                return std::make_unique<Int32Value>(std::stoi(s));
            case DataType::UINT32:
                return std::make_unique<UInt32Value>(std::stoul(s));
            case DataType::DOUBLE:
                return std::make_unique<DoubleValue>(std::stod(s));
            case DataType::DATA_TYPE:
            {
                return nullptr;
            }
            default:
                return nullptr;
            }
        }
        catch (const std::invalid_argument&)
        {
            return nullptr;
        }
        catch (const std::out_of_range&)
        {
            return nullptr;
        }
    }

protected:
    const std::string id;
    const std::string name;

    std::string value;
    DataType data_type;
};

}

#endif // TF_MODEL_PARAMETER_HPP
