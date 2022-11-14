// SPDX-License-Identifier: GPL-3.0-only

#include "block_graphics_view.h"

#include <QPainter>
#include <QBrush>

#include <QDebug>

#include <QTextStream>

#include <QMouseEvent>

#include <algorithm>

#include <tmdl/stdlib/stdlib.hpp>

#include "blocks/connector_object.h"


static const tmdl::stdlib::StandardLibrary BLOCK_LIBRARY;

// TODO - REMOVE!
#include <iostream>

BlockGraphicsView::BlockGraphicsView(QWidget* parent) :
    QGraphicsView(parent),
    selectedBlock(nullptr)
{
    // Set the scene
    setScene(new QGraphicsScene(this));
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

void BlockGraphicsView::mousePressEvent(QMouseEvent* event)
{
    // Determine if a block is under the mouse press event
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        const auto mappedPos = mapToScene(event->pos());
        BlockObject* block = findBlockForMousePress(mappedPos);

        if (block != selectedBlock && selectedBlock != nullptr)
        {
            std::cout << "Deselecting " << selectedBlock->get_block()->get_id() << std::endl;
            selectedBlock->setSelected(false);
            selectedBlock = nullptr;
            this->update();
        }

        if (block == nullptr)
        {
            return;
        }

        selectedBlock = block;
        selectedBlock->setSelected(true);

        std::cout << "Selecting " << selectedBlock->get_block()->get_id() << '\n';

        std::cout << "Mouse: (" << mappedPos.x() << ", " << mappedPos.y() << "), Block: (" << block->pos().x() << ", " << block->pos().y() << ")" << std::endl;

        const auto block_port = findBlockIOForMousePress(mappedPos, block);

        if (block_port)
        {
            portDragState.reset();
            portDragState.add_port(block_port.value());
        }
        else if (blockBodyContainsMouse(mappedPos, block))
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
}

void BlockGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    (void)event;
    mouseDragState.reset();

    if (portDragState.is_partial())
    {
        std::cout << "Partial Value!" << std::endl;

        const auto mappedPos = mapToScene(event->pos());
        BlockObject* block = findBlockForMousePress(mappedPos);
        const auto block_port = findBlockIOForMousePress(mappedPos, block);

        if (block_port)
        {
            portDragState.add_port(block_port.value());
        }

        if (portDragState.is_complete())
        {
            std::cout << "Adding connecting line!" << std::endl;

            const tmdl::Connection conn(
                portDragState.get_output().block->get_block()->get_id(),
                portDragState.get_output().port_count,
                portDragState.get_input().block->get_block()->get_id(),
                portDragState.get_input().port_count);

            model.add_connection(conn);

            ConnectorObject* conn_obj = new ConnectorObject(
                portDragState.get_output().block,
                portDragState.get_output().port_count,
                portDragState.get_input().block,
                portDragState.get_input().port_count);
            conn_obj->setParent(this);
            conn_obj->blockLocationUpdated();

            connect(
                portDragState.get_output().block,
                &BlockObject::sceneLocationUpdated,
                conn_obj,
                &ConnectorObject::blockLocationUpdated);

            connect(
                portDragState.get_input().block,
                &BlockObject::sceneLocationUpdated,
                conn_obj,
                &ConnectorObject::blockLocationUpdated);

            scene()->addItem(conn_obj);
        }
    }

    portDragState.reset();
}

void BlockGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (mouseDragState.hasBlock())
    {
        const QPointF newBlockPos = mapToScene(event->pos()) - mouseDragState.getBlock()->boundingRect().center() + mouseDragState.getOffset();
        const QPoint newBlockPosInt = snapMousePositionToGrid(newBlockPos.toPoint());
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
    const auto tmp = BLOCK_LIBRARY.create_block_from_name("limiter");
    model.add_block(tmp);

    // Create the block object
    BlockObject* block_obj = new BlockObject(tmp);
    block_obj->setParent(this);
    block_obj->setPos(mapToScene(QPoint(50, 50)));

    // Add the block to storage/tracking
    blocks.append(block_obj);
    scene()->addItem(block_obj);
}

BlockObject* BlockGraphicsView::findBlockForMousePress(const QPointF& pos)
{
    BlockObject* selected = nullptr;
    for (BlockObject* block : blocks)
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

std::optional<BlockObject::PortInformation> BlockGraphicsView::findBlockIOForMousePress(const QPointF& pos, const BlockObject* block)
{
    if (block == nullptr)
    {
        return std::nullopt;
    }

    return block->get_port_for_pos(pos);
}

bool BlockGraphicsView::blockBodyContainsMouse(const QPointF& pos, const BlockObject* block)
{
    if (block == nullptr) return false;
    return block->blockRectContainsPoint(pos);
}
