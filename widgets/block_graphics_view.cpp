// SPDX-License-Identifier: GPL-3.0-only

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
    scene()->setBackgroundBrush(QColor(163, 233, 255));
}

void BlockGraphicsView::mousePressEvent(QMouseEvent* event)
{
    // Determine if a block is under the mouse press event
    const auto mappedPos = mapToScene(event->pos());
    BaseBlockObject* block = findBlockForMousePress(mappedPos);

    if (block != nullptr)
    {
        // Update the blocks to bring the clicked block to the foreground
        const auto index = blocks.indexOf(block);
        if (index >= 0)
        {
            blocks.remove(index);
            blocks.insert(0, block);
        }

        scene()->removeItem(block);
        scene()->addItem(block);

        // Setup the drag state object
        mouseDragState.setState(
                    block->sceneBoundingRect().center() - mappedPos,
                    block);
    }
}

void BlockGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        mouseDragState.reset();
    }
}

void BlockGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton && mouseDragState.hasBlock())
    {
        const QPointF newBlockPos = mapToScene(event->pos()) - mouseDragState.getBlock()->boundingRect().center() + mouseDragState.getOffset();
        const QPoint newBlockPosInt = snapMousePositionToGrid(newBlockPos.toPoint());
        qDebug() << newBlockPosInt;
        mouseDragState.getBlock()->setPos(newBlockPosInt);
    }
}

QPoint BlockGraphicsView::snapMousePositionToGrid(const QPoint& input)
{
    return input - QPoint(
        input.x() % 10,
        input.y() % 10);
}

void BlockGraphicsView::addTestBlock()
{
    // Initialze the block
    BaseBlockObject* block = new BaseBlockObject(this);
    block->updateLocations();
    block->setPos(mapToScene(QPoint(50, 50)));

    // Add the block to storage/tracking
    blocks.append(block);
    scene()->addItem(block);
}

BaseBlockObject* BlockGraphicsView::findBlockForMousePress(const QPointF& pos)
{
    BaseBlockObject* selected = nullptr;
    for (BaseBlockObject* block : blocks)
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
