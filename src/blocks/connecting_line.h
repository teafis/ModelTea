#ifndef CONNECTING_LINE_H
#define CONNECTING_LINE_H

#include <QGraphicsObject>

#include "blocks/block_io_port.h"

#include <QPainter>

class ConnectorObject : public QGraphicsObject
{
public:
    ConnectorObject(const BlockIoPort* port_a, const BlockIoPort* port_b)
    {
        from_port = port_a;
        to_port = port_b;
    }

    virtual void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override
    {
        (void)widget;
        (void)option;

        //painter->setP
    }

    virtual QRectF boundingRect() const override;

public slots:
    void updateLocations()
    {

    }

protected:
    const BlockIoPort* from_port;
    const BlockIoPort* to_port;
};

#endif // CONNECTING_LINE_H
