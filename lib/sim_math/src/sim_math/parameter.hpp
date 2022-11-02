// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_SIM_MATH_PARAMETER_HPP
#define TF_SIM_MATH_PARAMETER_HPP

#include <memory>
#include <string>

#include <sim_math/data_types.hpp>
#include <sim_math/signal.hpp>

namespace sim_math
{

class Parameter
{
public:
    Parameter(
        const std::string& param_id,
        const std::string& name,
        const std::string& description,
        const DataType& data_type,
        const std::string& default_value);

    inline const std::string& get_id() const;

    inline const std::string& get_name() const;

    inline const std::string& get_description() const;

    bool set_value(const std::string& new_value);

    std::shared_ptr<Signal> get_parameter_value() const;

protected:
    std::shared_ptr<Signal> parse_parameter(const std::string& s) const;

private:
    const std::string identifier;
    const std::string name;
    const std::string description;
    std::string value;
    const DataType data_type;
};

}

#endif // TF_SIM_MATH_PARAMETER_HPP
