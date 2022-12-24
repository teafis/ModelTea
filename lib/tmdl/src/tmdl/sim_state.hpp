// SPDX-License-Identifier: GPL-3.0-only

#ifndef TF_MODEL_SIM_STATE_HPP
#define TF_MODEL_SIM_STATE_HPP

namespace tmdl
{

class SimState
{
public:
    SimState(const double dt);

    double get_time() const;

    double get_dt() const;

    void set_time(const double& t);

private:
    double time;
    double dt;
};

}

#endif // TF_MODEL_SIM_STATE_HPP
