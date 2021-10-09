#include "block_graphics_view.h"

#include <QPainter>
#include <QBrush>

#include <QDebug>

#include <QTextStream>

#include <QMouseEvent>

#include <algorithm>

BlockGraphicsView::BlockGraphicsView(QWidget* parent) :
    QGraphicsView(parent)
{
    // Set the scene
    setScene(new QGraphicsScene(this));

    // Add a test block
    scene()->addItem(&base_block);
    qInfo() << sceneRect() << ", " << visibleRegion().boundingRect();
}

void BlockGraphicsView::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    qInfo() << "Mouse Press";
    base_block.setX(0.0);
    base_block.setY(0.0);
}

void BlockGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    qInfo() << "Mouse Release";
}

void BlockGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    qInfo() << "Mouse Move: " << event->x() << ", " << event->y() << ", " << sceneRect() << ", " << visibleRegion().boundingRect();
    base_block.setX(event->x() - event->x() % 10);
    base_block.setY(event->y() - event->y() % 10);
}

void BlockGraphicsView::resizeEvent(QResizeEvent* event)
{
    qInfo() << "Resize: " << event->size();
    QRectF new_rect = sceneRect();
    new_rect.setHeight(
                std::max(
                    sceneRect().height(),
                    static_cast<double>(event->size().height())));
    new_rect.setWidth(
                std::max(
                    sceneRect().width(),
                    static_cast<double>(event->size().width())));
    setSceneRect(new_rect);
    qInfo() << "  " << sceneRect();
}
