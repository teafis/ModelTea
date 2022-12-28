// SPDX-License-Identifier: GPL-3.0-only

#include "connector_object.h"

ConnectorObject::ConnectorObject(
    const BlockObject* from_block,
    const size_t from_port,
    const BlockObject* to_block,
    const size_t to_port) :
    from_block(from_block),
    to_block(to_block),
    from_port(from_port),
    to_port(to_port)
{
    if (from_block == nullptr || to_block == nullptr)
    {
        throw 1;
    }

    // Set the provided parent to help with destruction
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void ConnectorObject::paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    (void)widget;
    (void)option;

    if (isSelected())
    {
        painter->setPen(QPen(Qt::blue, 5.0));
    }
    else
    {
        painter->setPen(QPen(Qt::black, 3.0));
    }
    painter->setBrush(Qt::transparent);

    const auto pts = getLinePoints();
    for (int i = 1; i < pts.size(); ++i)
    {
        painter->drawLine(pts[i-1], pts[i]);
    }
}

bool ConnectorObject::positionOnLine(const QPointF& localCoords) const
{
    const auto linePoints = getLinePoints();
    const double THRESHOLD = 5.0;

    for (int i = 1; i < linePoints.size(); ++i)
    {
        const auto& a = linePoints[i - 1];
        const auto& b = linePoints[i];

        const double dx = b.x() - a.x();
        const double dy = b.y() - a.y();

        const double dist = std::sqrt(std::pow(dx, 2.0) + std::pow(dy, 2.0));

        const double local_dx = a.x() - localCoords.x();
        const double local_dy = a.y() - localCoords.y();

        if (dist < 1.0)
        {
            if (std::sqrt(std::pow(local_dx, 2.0) + std::pow(local_dy, 2.0)) < THRESHOLD)
            {
                return true;
            }

            continue;
        }

        const double dxn = dx / dist;
        const double dyn = dy / dist;

        const double denom = dx * dxn + dy * dyn;

        const double line_param = (-dyn * local_dy - dxn * local_dx) / denom;
        const double dist_from_line = (dx * local_dy + dy * local_dx) / denom;

        if (line_param >= 0.0 && line_param <= 1.0 && std::abs(dist_from_line) < THRESHOLD)
        {
            return true;
        }
    }

    return false;
}

QRectF ConnectorObject::boundingRect() const
{
    return QRectF(
        0.0,
        0.0,
        std::abs(loc_from.x() - loc_to.x()),
        std::abs(loc_from.y() - loc_to.y()));
}

bool ConnectorObject::isValidConnection() const
{
    if (from_port >= from_block->getNumPortsForType(BlockObject::PortType::OUTPUT))
    {
        return false;
    }
    else if (to_port >= to_block->getNumPortsForType(BlockObject::PortType::INPUT))
    {
        return false;
    }
    else
    {
        return true;
    }
}

const BlockObject* ConnectorObject::get_from_block() const
{
    return from_block;
}

const BlockObject* ConnectorObject::get_to_block() const
{
    return to_block;
}

size_t ConnectorObject::get_from_port() const
{
    return from_port;
}

size_t ConnectorObject::get_to_port() const
{
    return to_port;
}

void ConnectorObject::blockLocationUpdated()
{
    if (!isValidConnection())
    {
        return;
    }

    loc_to = to_block->mapToScene(to_block->getInputPortLocation(to_port));
    loc_from = from_block->mapToScene(from_block->getOutputPortLocation(from_port));

    setVisible(false);
    setPos(std::min(loc_from.x(), loc_to.x()), std::min(loc_from.y(), loc_to.y()));

    loc_from = mapFromScene(loc_from);
    loc_to = mapFromScene(loc_to);

    update();
    setVisible(true);
}

QVector<QPointF> ConnectorObject::getLinePoints() const
{
    const auto halfway = (loc_from + loc_to) / 2.0;

    return {
        loc_from,
        QPointF(halfway.x(), loc_from.y()),
        QPointF(halfway.x(), loc_to.y()),
        loc_to
    };
}
