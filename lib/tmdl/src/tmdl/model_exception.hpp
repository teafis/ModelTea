// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_EXCEPTION_HPP
#define TF_MODEL_EXCEPTION_HPP

#include <string>


namespace tmdl
{

class ModelException
{
public:
    ModelException(const std::string& msg) : msg(msg)
    {
        // Empty Constructor
    }

    const std::string& what() const
    {
        return msg;
    }

    virtual ~ModelException()
    {
        // Empty Destructor
    }

protected:
    std::string msg;
};

}

#endif // TF_MODEL_EXCEPTION_HPP
