// SPDX-License-Identifier: GPL-3.0-only

#ifndef PORT_DRAG_STATE_H
#define PORT_DRAG_STATE_H

#include "blocks/block_object.h"

#include <optional>


class PortDragState
{
public:
    void add_port(const BlockObject::PortInformation port)
    {
        switch (port.type)
        {
        case BlockObject::PortType::INPUT:
            port_input = port;
            break;
        case BlockObject::PortType::OUTPUT:
            port_output = port;
            break;
        default:
            return;
        }
    }

    bool is_partial() const
    {
        return port_output || port_input;
    }

    bool is_complete() const
    {
        return port_output && port_input;
    }

    const BlockObject::PortInformation& get_output() const
    {
        return port_output.value();
    }

    const BlockObject::PortInformation& get_input() const
    {
        return port_input.value();
    }

    void reset()
    {
        port_output = std::nullopt;
        port_input = std::nullopt;
    }

protected:
    std::optional<BlockObject::PortInformation> port_output;
    std::optional<BlockObject::PortInformation> port_input;
};

#endif // PORT_DRAG_STATE_H
