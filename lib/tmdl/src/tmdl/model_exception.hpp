// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_EXCEPTION_HPP
#define TF_MODEL_EXCEPTION_HPP

#include <string>


namespace tmdl
{

class ModelException
{
public:
    ModelException(const std::string& msg) : _msg(msg)
    {
        // Empty Constructor
    }

    const std::string& what() const
    {
        return _msg;
    }

    virtual ~ModelException()
    {
        // Empty Destructor
    }

protected:
    std::string _msg;
};

class ExecutionException : public ModelException
{
public:
    ExecutionException(
        const std::string& msg,
        const size_t id) :
        ModelException(msg),
        _id(id)
    {
        // Empty Constructor
    }

    size_t get_id() const
    {
        return _id;
    }

protected:
    size_t _id;
};

}

#endif // TF_MODEL_EXCEPTION_HPP
