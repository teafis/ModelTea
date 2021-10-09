#include "base_block.h"

#include <QPainter>
#include <QWidget>

BaseBlock::BaseBlock(QGraphicsItem* parent) :
    QGraphicsObject(parent)
{
    // Parent
}

void BaseBlock::paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem *option,
        QWidget* widget)
{
    painter->setBrush(Qt::gray);
    painter->drawRect(boundingRect());
}

QRectF BaseBlock::boundingRect() const
{
    return QRectF(0, 0, 50, 50);
}
