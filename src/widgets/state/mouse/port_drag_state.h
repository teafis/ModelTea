// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCK_PORT_DRAG_STATE_H
#define BLOCK_PORT_DRAG_STATE_H

#include "blocks/block_object.h"
#include "blocks/connector_object.h"
#include "mouse_state_base.h"


class PortDragState : public MouseStateBase
{
public:
    PortDragState(const BlockObject::PortInformation& port);

    ~PortDragState();

    void add_port(const BlockObject::PortInformation& port);

    bool is_partial() const;

    bool is_complete() const;

    const BlockObject::PortInformation& get_output() const;

    const BlockObject::PortInformation& get_input() const;

    void updateMouseLocation(QPointF loc);

    void reset();

    ConnectorObject* get_connector() const;

protected:
    std::optional<BlockObject::PortInformation> port_output;
    std::optional<BlockObject::PortInformation> port_input;
    ConnectorObject* connector;
};

#endif // BLOCK_PORT_DRAG_STATE_H
