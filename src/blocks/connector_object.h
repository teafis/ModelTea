// SPDX-License-Identifier: GPL-3.0-only

#ifndef CONNECTING_LINE_H
#define CONNECTING_LINE_H

#include <QGraphicsObject>

#include "blocks/block_object.h"

#include <QPainter>

class ConnectorObject : public QGraphicsObject
{
    Q_OBJECT

public:
    ConnectorObject(
        const BlockObject* from_block,
        const size_t from_port,
        const BlockObject* to_block,
        const size_t to_port);

    virtual void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

    bool positionOnLine(const QPointF& localCoords) const;

    virtual QRectF boundingRect() const override;

    bool isValidConnection() const;

    const BlockObject* get_from_block() const;

    const BlockObject* get_to_block() const;

    size_t get_from_port() const;

    size_t get_to_port() const;

public slots:
    void blockLocationUpdated();

protected:
    QVector<QPointF> getLinePoints() const;

protected:
    const BlockObject* from_block;
    const BlockObject* to_block;
    const size_t from_port;
    const size_t to_port;

    QPointF loc_from;
    QPointF loc_to;
};

#endif // CONNECTING_LINE_H
