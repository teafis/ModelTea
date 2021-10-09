#include "base_block.h"

#include <QPainter>
#include <QWidget>

#include <QDebug>

BaseBlock::BaseBlock(QGraphicsItem* parent) :
    QGraphicsObject(parent),
    num_inputs(0),
    num_outputs(0),
    name("TEMP")
{
    // Empty Constructor
}

BaseBlock::BaseBlock(QObject* parent) :
    num_inputs(0),
    num_outputs(0),
    name("TEMP")
{
    setParent(parent);
}

void BaseBlock::paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::black));
    painter->setBrush(Qt::gray);
    painter->drawRect(boundingRect());

    qInfo() << x() << ", " << y();
}

QRectF BaseBlock::boundingRect() const
{
    return QRectF(
                0,
                0,
                50,
                50);
}
