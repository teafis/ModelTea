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
    scene()->setBackgroundBrush(Qt::white);
}

void BlockGraphicsView::mousePressEvent(QMouseEvent* event)
{
    const auto mappedPos = mapToScene(event->pos());
    BaseBlock* block = findBlockForMousePress(mappedPos);

    if (block != nullptr)
    {
        mouseDragState.setState(
                    block->sceneBoundingRect().center() - mappedPos,
                    block);
    }
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
                    mapToScene(event->pos()) - mouseDragState.getBlock()->boundingRect().center() + mouseDragState.getOffset());
    }
}

void BlockGraphicsView::addTestBlock()
{
    blocks.append(new BaseBlock(this));
    scene()->addItem(blocks.last());
}

BaseBlock* BlockGraphicsView::findBlockForMousePress(const QPointF& pos)
{
    BaseBlock* selected = nullptr;
    for (BaseBlock *const block : blocks)
    {
        const QRectF boundingRect = block->sceneBoundingRect();
        if (boundingRect.contains(pos))
        {
            selected = block;
            break;
        }
    }
    return selected;
}
