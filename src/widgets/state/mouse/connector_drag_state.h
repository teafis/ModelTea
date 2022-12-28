// SPDX-License-Identifier: GPL-3.0-only

#ifndef CONNECTOR_MOUSE_DRAG_STATE_H
#define CONNECTOR_MOUSE_DRAG_STATE_H

#include <QPointF>

#include "blocks/connector_object.h"
#include "mouse_state_base.h"

class ConnectorDragState : public MouseStateBase
{
public:
    ConnectorDragState(
        ConnectorObject* connector,
        const QPointF& offset,
        const QPoint& original);

    ConnectorObject* getConnector() const;

protected:
    ConnectorObject* connector;
};

#endif // CONNECTOR_MOUSE_DRAG_STATE_H
