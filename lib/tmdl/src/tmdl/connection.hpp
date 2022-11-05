// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_CONNECTION_HPP
#define TF_MODEL_CONNECTION_HPP

#include <cstdlib>

namespace tmdl
{

class Connection
{
public:

protected:
    size_t from_id;
    size_t from_port;
    size_t to_id;
    size_t to_port;
};

}

#endif // TF_MODEL_CONNECTION_HPP
