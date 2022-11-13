// SPDX-License-Identifier: GPL-3.0-only

#ifndef PORT_DRAG_STATE_H
#define PORT_DRAG_STATE_H

#include "blocks/block_io_port.h"

class PortDragState
{
public:
    void add_port(const BlockIoPort* port)
    {
        if (port == nullptr)
        {
            return;
        }

        switch (port->get_type())
        {
        case BlockIoPort::PortType::INPUT:
            port_input = port;
            break;
        case BlockIoPort::PortType::OUTPUT:
            port_output = port;
            break;
        default:
            return;
        }
    }

    bool complete_state() const
    {
        return port_output != nullptr && port_input != nullptr;
    }

    const BlockIoPort* get_output() const
    {
        return port_output;
    }

    const BlockIoPort* get_input() const
    {
        return port_input;
    }

protected:
    const BlockIoPort* port_output;
    const BlockIoPort* port_input;
};

#endif // PORT_DRAG_STATE_H
