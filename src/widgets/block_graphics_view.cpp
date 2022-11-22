// SPDX-License-Identifier: GPL-3.0-only

#include "block_graphics_view.h"

#include <QPainter>
#include <QBrush>

#include <QDebug>

#include <QTextStream>
#include <QMessageBox>

#include <QMouseEvent>

#include <algorithm>

#include <tmdl/stdlib/tmdl_stdlib.hpp>

#include "blocks/connector_object.h"

#include "state/block_drag_state.h"
#include "state/port_drag_state.h"

#include "windows/parameter_dialog.h"
#include "windows/block_library.h"


BlockGraphicsView::BlockGraphicsView(QWidget* parent) :
    QGraphicsView(parent),
    selectedBlock(nullptr)
{
    // Set the scene
    setScene(new QGraphicsScene(this));
    setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Set the library
    library = std::make_shared<tmdl::stdlib::StandardLibrary>();
}

void BlockGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (executor != nullptr)
    {
        return;
    }

    // Determine if a block is under the mouse press event
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        const auto mappedPos = mapToScene(event->pos());
        BlockObject* block = findBlockForMousePress(mappedPos);

        if (block != selectedBlock && selectedBlock != nullptr)
        {
            selectedBlock->setSelected(false);
            selectedBlock = nullptr;
            update();
        }

        if (block == nullptr)
        {
            return;
        }

        selectedBlock = block;
        selectedBlock->setSelected(true);

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
    if (executor != nullptr)
    {
        return;
    }

    if (const auto* mouseDragState = dynamic_cast<BlockDragState*>(mouseState.get()); mouseDragState != nullptr)
    {
        const QPointF newBlockPos = mapToScene(
            event->pos()) - mouseDragState->getBlock()->boundingRect().center() + mouseDragState->getOffset();
        const QPoint newBlockPosInt = snapMousePositionToGrid(newBlockPos.toPoint());
        mouseDragState->getBlock()->setPos(newBlockPosInt);
    }
}

void BlockGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (executor != nullptr)
    {
        return;
    }

    if (auto* portDragState = dynamic_cast<PortDragState*>(mouseState.get()); portDragState != nullptr)
    {
        const auto mappedPos = mapToScene(event->pos());
        BlockObject* block = findBlockForMousePress(mappedPos);
        const auto block_port = findBlockIOForMousePress(mappedPos, block);

        if (block_port)
        {
            portDragState->add_port(block_port.value());
        }

        if (portDragState->is_complete())
        {
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
    if (executor != nullptr)
    {
        return;
    }

    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        const auto mappedPos = mapToScene(event->pos());
        BlockObject* block = findBlockForMousePress(mappedPos);

        if (block != nullptr)
        {
            ParameterDialog* dialog = new ParameterDialog(block, this);
            dialog->exec();

            for (auto ptr : scene()->items())
            {
                if (auto c = dynamic_cast<ConnectorObject*>(ptr); c != nullptr)
                {
                    if (!c->isValidConnection())
                    {
                        model.remove_connection(
                            c->get_to_block()->get_block()->get_id(),
                            c->get_to_port());
                        c->deleteLater();
                    }
                }
            }

            for (auto ptr : scene()->items())
            {
                if (auto b = dynamic_cast<BlockObject*>(ptr); b != nullptr)
                {
                    emit b->sceneLocationUpdated();
                }
            }

            update();
        }
    }
}

QPoint BlockGraphicsView::snapMousePositionToGrid(const QPoint& input)
{
    return input - QPoint(
        input.x() % 10,
        input.y() % 10);
}

void BlockGraphicsView::removeSelectedBlock()
{
    if (executor != nullptr)
    {
        return;
    }

    if (selectedBlock != nullptr)
    {
        model.remove_block(selectedBlock->get_block()->get_id());
        scene()->removeItem(selectedBlock);
        selectedBlock->deleteLater();
        selectedBlock = nullptr;
    }
}

void BlockGraphicsView::updateModel()
{
    if (executor != nullptr)
    {
        return;
    }

    if (model.update_block())
    {
        for (auto* item : scene()->items())
        {
            auto* block = dynamic_cast<BlockObject*>(item);
            if (block != nullptr)
            {
                block->update();
            }
        }
    }
}

void BlockGraphicsView::generateExecutor()
{
    tmdl::ConnectionManager connections;
    tmdl::VariableManager manager;

    try
    {
    executor = model.get_execution_interface(
        connections,
        manager);
    }
    catch (const tmdl::ModelException& ex)
    {
        auto* msg = new QMessageBox(this);
        msg->setText(ex.what().c_str());
        msg->setWindowTitle("Parameter Error");
        msg->exec();

        executor = nullptr;
        return;
    }

    for (auto* c : children())
    {
        auto* dialog = dynamic_cast<BlockLibrary*>(c);
        if (dialog != nullptr)
        {
            dialog->close();
        }
    }
}

void BlockGraphicsView::clearExecutor()
{
    executor = nullptr;
}

void BlockGraphicsView::showLibrary()
{
    BlockLibrary* lib = new BlockLibrary(this);
    lib->set_library(library);

    connect(
        lib,
        &BlockLibrary::blockSelected,
        this,
        &BlockGraphicsView::addBlock);

    lib->show();
}

void BlockGraphicsView::addBlock(QString s)
{
    if (executor != nullptr)
    {
        return;
    }

    // Initialze the block
    const auto tmp = library->create_block_from_name(s.toStdString());
    model.add_block(tmp);

    // Create the block object
    BlockObject* block_obj = new BlockObject(tmp);
    block_obj->setParent(this);
    block_obj->setPos(mapToScene(QPoint(50, 50)));

    // Add the block to storage/tracking
    scene()->addItem(block_obj);
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

std::optional<BlockObject::PortInformation> BlockGraphicsView::findBlockIOForMousePress(
    const QPointF& pos,
    const BlockObject* block)
{
    if (block == nullptr)
    {
        return std::nullopt;
    }

    return block->get_port_for_pos(pos);
}

bool BlockGraphicsView::blockBodyContainsMouse(
    const QPointF& pos,
    const BlockObject* block)
{
    if (block == nullptr) return false;
    return block->blockRectContainsPoint(pos);
}
