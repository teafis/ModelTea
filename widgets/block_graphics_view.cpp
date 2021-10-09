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

    // Add a new block to the scene
    blocks.append(new BaseBlock(this));
    blocks.append(new BaseBlock(this));

    // Add a test block
    for (const auto& block : blocks)
    {
        scene()->addItem(block);
    }
}

void BlockGraphicsView::mousePressEvent(QMouseEvent* event)
{
    mouseDragState.setState(
                event,
                findBlockForMousePress(event->pos()));
}

void BlockGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    mouseDragState.reset();
}

void BlockGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (mouseDragState.hasBlock())
    {
        mouseDragState.getBlock()->setPos(
                    event->pos() - mouseDragState.getBlock()->boundingRect().center() + mouseDragState.getOffset());
    }
}

void BlockGraphicsView::resizeEvent(QResizeEvent* event)
{
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
}

BaseBlock* BlockGraphicsView::findBlockForMousePress(const QPoint& pos)
{
    BaseBlock* selected = nullptr;
    for (BaseBlock *const block : blocks)
    {
        QRectF boundingRect = block->sceneBoundingRect();
        if (boundingRect.contains(pos))
        {
            selected = block;
            break;
        }
    }
    return selected;
}
