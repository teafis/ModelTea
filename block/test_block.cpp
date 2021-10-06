#include "test_block.h"

#include <QPainter>
#include <QWidget>

TestBlock::TestBlock(QGraphicsItem* parent) :
    QGraphicsObject(parent)
{
    // Parent
}

void TestBlock::paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem *option,
        QWidget* widget)
{
    painter->setBrush(Qt::gray);
    painter->drawRect(widget->rect());
}

QRectF TestBlock::boundingRect() const
{
    return QRectF(0, 0, 150, 150);
}
