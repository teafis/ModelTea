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
    }

    virtual void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override
    {
        (void)widget;
        (void)option;

        painter->setPen(QPen(Qt::black, 3.0));
        painter->setBrush(Qt::transparent);

        const auto halfway = (loc_from + loc_to) / 2.0;

        painter->drawLine(loc_from, QPointF(halfway.x(), loc_from.y()));
        painter->drawLine(loc_to, QPointF(halfway.x(), loc_to.y()));
        painter->drawLine(QPointF(halfway.x(), loc_from.y()), QPointF(halfway.x(), loc_to.y()));
    }

    virtual QRectF boundingRect() const override
    {
        return QRectF(0.0, 0.0, std::abs(loc_from.x() - loc_to.x()), std::abs(loc_from.y() - loc_to.y()));
    }

public slots:
    void blockLocationUpdated()
    {
        loc_to = to_block->mapToScene(to_block->getInputPortLocation(to_port));
        loc_from = from_block->mapToScene(from_block->getOutputPortLocation(from_port));

        setVisible(false);
        setPos(std::min(loc_from.x(), loc_to.x()), std::min(loc_from.y(), loc_to.y()));

        loc_from = mapFromScene(loc_from);
        loc_to = mapFromScene(loc_to);

        update();
        setVisible(true);
    }

protected:
    const BlockObject* from_block;
    const BlockObject* to_block;
    const size_t from_port;
    const size_t to_port;

    QPointF loc_from;
    QPointF loc_to;
};

#endif // CONNECTING_LINE_H
