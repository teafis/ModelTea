// SPDX-License-Identifier: GPL-3.0-only

#ifndef CONNECTOR_OBJECT_H
#define CONNECTOR_OBJECT_H

#include <QGraphicsObject>
#include <QWidget>

#include <memory>

class ConnectorObject : public QGraphicsObject {
    Q_OBJECT

public:
    ConnectorObject();

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    virtual bool positionOnLine(const QPointF& localCoords) const;

    virtual QRectF boundingRect() const override;

public slots:
    void updateLocA(QPointF loc);

    void updateLocB(QPointF loc);

    void updateLocations(QPointF a, QPointF b);

protected:
    void updateLocationValues();

    virtual QColor getLineColor(QWidget* widget) const;

    virtual double getLineWidth() const;

    virtual QVector<QPointF> getLinePoints() const;

protected:
    QPointF loc_a;
    QPointF loc_b;
};

#endif // CONNECTOR_OBJECT_H
