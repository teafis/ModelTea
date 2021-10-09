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
    qInfo() << sceneRect() << ", " << visibleRegion().boundingRect();
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
    qInfo() << "Mouse Move: " << event->pos() << ", " << sceneRect() << ", " << visibleRegion().boundingRect();

    BaseBlock* selectedBlock = findBlockForMousePress(event->pos());
    if (selectedBlock != nullptr)
    {
        selectedBlock->setPos(event->pos() - selectedBlock->boundingRect().center());
    }
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
