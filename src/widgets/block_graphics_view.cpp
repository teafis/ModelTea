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

#include "state/block_drag_state.h"
#include "state/port_drag_state.h"

#include "windows/parameter_dialog.h"


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
            mouseState = std::make_unique<PortDragState>(block_port.value());
        }
        else if (blockBodyContainsMouse(mappedPos, block))
        {
            // Update the blocks to bring the clicked block to the foreground
            scene()->removeItem(block);
            scene()->addItem(block);

            // Setup the drag state object
            mouseState = std::make_unique<BlockDragState>(
                block,
                block->sceneBoundingRect().center() - mappedPos);
        }
    }
}

void BlockGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (const auto* mouseDragState = dynamic_cast<BlockDragState*>(mouseState.get()); mouseDragState != nullptr)
    {
        const QPointF newBlockPos = mapToScene(event->pos()) - mouseDragState->getBlock()->boundingRect().center() + mouseDragState->getOffset();
        const QPoint newBlockPosInt = snapMousePositionToGrid(newBlockPos.toPoint());
        mouseDragState->getBlock()->setPos(newBlockPosInt);
    }
}

void BlockGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (auto* portDragState = dynamic_cast<PortDragState*>(mouseState.get()); portDragState != nullptr)
    {
        std::cout << "Partial Value!" << std::endl;

        const auto mappedPos = mapToScene(event->pos());
        BlockObject* block = findBlockForMousePress(mappedPos);
        const auto block_port = findBlockIOForMousePress(mappedPos, block);

        if (block_port)
        {
            portDragState->add_port(block_port.value());
        }

        if (portDragState->is_complete())
        {
            std::cout << "Adding connecting line!" << std::endl;

            const tmdl::Connection conn(
                portDragState->get_output().block->get_block()->get_id(),
                portDragState->get_output().port_count,
                portDragState->get_input().block->get_block()->get_id(),
                portDragState->get_input().port_count);

            model.add_connection(conn);

            ConnectorObject* conn_obj = new ConnectorObject(
                portDragState->get_output().block,
                portDragState->get_output().port_count,
                portDragState->get_input().block,
                portDragState->get_input().port_count);
            conn_obj->setParent(this);
            conn_obj->blockLocationUpdated();

            connect(
                portDragState->get_output().block,
                &BlockObject::sceneLocationUpdated,
                conn_obj,
                &ConnectorObject::blockLocationUpdated);

            connect(
                portDragState->get_input().block,
                &BlockObject::sceneLocationUpdated,
                conn_obj,
                &ConnectorObject::blockLocationUpdated);

            connect(
                portDragState->get_output().block,
                &BlockObject::destroyed,
                conn_obj,
                &ConnectorObject::deleteLater);
            connect(
                portDragState->get_input().block,
                &BlockObject::destroyed,
                conn_obj,
                &ConnectorObject::deleteLater);

            scene()->addItem(conn_obj);
        }
    }

    mouseState = nullptr;
}

void BlockGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        const auto mappedPos = mapToScene(event->pos());
        BlockObject* block = findBlockForMousePress(mappedPos);

        if (block != nullptr)
        {
            ParameterDialog* dialog = new ParameterDialog(block, this);
            dialog->show();
        }
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
    scene()->addItem(block_obj);
}

void BlockGraphicsView::removeSelectedBlock()
{
    if (selectedBlock != nullptr)
    {
        model.remove_block(selectedBlock->get_block()->get_id());
        scene()->removeItem(selectedBlock);
        selectedBlock->deleteLater();
        selectedBlock = nullptr;
    }
}

BlockObject* BlockGraphicsView::findBlockForMousePress(const QPointF& pos)
{
    for (auto itm : scene()->items())
    {
        BlockObject* blk = dynamic_cast<BlockObject*>(itm);
        if (blk != nullptr && blk->sceneBoundingRect().contains(pos))
        {
            return blk;
        }
    }

    return nullptr;
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
