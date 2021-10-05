#include "block_graphics_view.h"

#include <QPainter>
#include <QBrush>

#include <QDebug>

BlockGraphicsView::BlockGraphicsView(QWidget* parent) :
    QGraphicsView(parent)
{
    // Empty Constructor
}

void BlockGraphicsView::render(
        QPainter *painter,
        const QRectF &target,
        const QRect &source,
        Qt::AspectRatioMode aspectRatioMode)
{
    qDebug() << "Rendering!";
    painter->setBrush(QBrush(Qt::blue));
    painter->drawRect(target);
}
