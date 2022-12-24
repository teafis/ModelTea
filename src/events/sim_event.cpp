// SPDX-License-Identifier: GPL-3.0-only

#include "sim_event.h"

SimEvent::SimEvent(const SimEvent::EventType t) :
    _type(t)
{
    // Empty Constructor
}


SimEvent::EventType SimEvent::event() const
{
    return _type;
}
