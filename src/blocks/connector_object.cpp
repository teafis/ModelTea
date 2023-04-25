// SPDX-License-Identifier: GPL-3.0-only

#include "connector_object.h"

#include <QPainter>


ConnectorObject::ConnectorObject()
{
    // Empty Constructor
}

void ConnectorObject::paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    (void)widget;
    (void)option;

    painter->setPen(QPen(getLineColor(), getLineWidth()));
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
        std::abs(loc_b.x() - loc_a.x()),
        std::abs(loc_b.y() - loc_a.y()));
}

void ConnectorObject::updateLocA(QPointF loc)
{
    loc_a = mapFromScene(loc);
    updateLocationValues();
}

void ConnectorObject::updateLocB(QPointF loc)
{
    loc_b = mapFromScene(loc);
    updateLocationValues();
}

void ConnectorObject::updateLocations(QPointF a, QPointF b)
{
    loc_a = mapFromScene(a);
    loc_b = mapFromScene(b);
    updateLocationValues();
}

void ConnectorObject::updateLocationValues()
{
    const auto loc_a_scene = mapToScene(loc_a);
    const auto loc_b_scene = mapToScene(loc_b);

    setVisible(false);
    setPos(
        std::min(loc_b_scene.x(), loc_a_scene.x()),
        std::min(loc_b_scene.y(), loc_a_scene.y()));

    loc_a = mapFromScene(loc_a_scene);
    loc_b = mapFromScene(loc_b_scene);

    update();
    setVisible(true);
}

QColor ConnectorObject::getLineColor() const
{
    return Qt::red;
}

double ConnectorObject::getLineWidth() const
{
    return 1.0;
}

QVector<QPointF> ConnectorObject::getLinePoints() const
{
    const auto halfway = (loc_a + loc_b) / 2.0;

    return {
        loc_b,
        QPointF(halfway.x(), loc_b.y()),
        QPointF(halfway.x(), loc_a.y()),
        loc_a
    };
}
