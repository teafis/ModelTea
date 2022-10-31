#ifndef TF_SIM_MATH_LIMITER_H
#define TF_SIM_MATH_LIMITER_H

#include <cstdlib>

#include <vector>
#include <string>

#include "sim_codegen_state.hpp"
#include "sim_types.hpp"

namespace sim_math
{

class Limiter
{
public:
    size_t num_inputs() const;

    size_t num_outputs() const;

    DataType get_output_port_type(const size_t port_num) const;

    DataType get_input_port_type(const size_t port_num) const;

    void set_input_port_type(
        const size_t port_num,
        const DataType port_type) const;

    std::vector<std::string> emit_code(CodeGenState& state) const;
};

}

#endif // TF_SIM_MATH_LIMITER_H
