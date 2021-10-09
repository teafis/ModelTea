#include "block_graphics_view.h"

#include <QPainter>
#include <QBrush>

#include <QDebug>

#include <QTextStream>

#include <QMouseEvent>

BlockGraphicsView::BlockGraphicsView(QWidget* parent) :
    QGraphicsView(parent)
{
    // Set the scene
    setScene(new QGraphicsScene(this));

    // Add a test block
    scene()->addItem(&base_block);
}

void BlockGraphicsView::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    qInfo() << "Mouse Press";
}

void BlockGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    qInfo() << "Mouse Release";
}

void BlockGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    qInfo() << "Mouse Move: " << event->x() << ", " << event->y();
    base_block.setX(event->x() - event->x() % 10);
    base_block.setY(event->y() - event->y() % 10);
}
