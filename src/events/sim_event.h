// SPDX-License-Identifier: GPL-3.0-only

#ifndef SIMEVENT_H
#define SIMEVENT_H

#include <cstddef>

class SimEvent
{
public:
    enum class EventType
    {
        Reset,
        Close,
        Create,
        Step
    };

public:
    explicit SimEvent(const EventType t);

    EventType event() const;

private:
    EventType _type;
};

#endif // SIMEVENT_H
