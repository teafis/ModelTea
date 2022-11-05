#ifndef TF_SIM_MATH_COMPUTE_RESULT_HPP
#define TF_SIM_MATH_COMPUTE_RESULT_HPP

#include <string>

namespace sim_math
{

class ComputeResult
{
public:
    virtual bool success() const = 0;
};

class ComputeSuccess : public ComputeResult
{
public:
    bool success() const override
    {
        return true;
    }
};

class ComputeError : public ComputeResult
{
public:
    ComputeError(const std::string& msg) : msg(msg)
    {
        // Empty Constructor
    }

    bool success() const override
    {
        return false;
    }

    const std::string& what() const
    {
        return msg;
    }

protected:
    std::string msg;
};

}

#endif // TF_SIM_MATH_COMPUTE_RESULT_HPP
