// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_EXCEPTION_HPP
#define TF_MODEL_EXCEPTION_HPP

#include <string>
#include <exception>


namespace tmdl
{

class ModelException : public std::exception
{
public:
    ModelException(const std::string& msg);

    const char* what() const noexcept override;

    virtual ~ModelException();

protected:
    std::string _msg;
};

class ExecutionException : public ModelException
{
public:
    ExecutionException(
        const std::string& msg,
        const size_t id);

    size_t get_id() const;

protected:
    size_t _id;
};

}

#endif // TF_MODEL_EXCEPTION_HPP
