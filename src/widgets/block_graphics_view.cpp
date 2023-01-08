// SPDX-License-Identifier: GPL-3.0-only

#include "block_graphics_view.h"

#include <QtGlobal>

#include <QPainter>
#include <QBrush>

#include <QDebug>

#include <QTextStream>
#include <QMessageBox>

#include <QMouseEvent>

#include <algorithm>
#include <iomanip>

#include <tmdl/library_manager.hpp>
#include <tmdl/model_exception.hpp>

#include "blocks/connector_object.h"

#include "state/mouse/block_drag_state.h"
#include "state/mouse/port_drag_state.h"

#include "dialogs/block_parameters_dialog.h"
#include "dialogs/connection_parameters_dialog.h"

#include <nlohmann/json.hpp>


BlockGraphicsView::BlockGraphicsView(QWidget* parent) :
    QGraphicsView(parent),
    selectedItem(nullptr)
{
    // Attempt to create a new model
    model = tmdl::LibraryManager::get_instance().default_model_library()->create_model();

    // Set the scene
    setScene(new QGraphicsScene(this));
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

void BlockGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (!isEnabled())
    {
        return;
    }

    // Determine if a block is under the mouse press event
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        const auto mappedPos = mapToScene(event->pos());

        BlockObject* const selectedBlock = dynamic_cast<BlockObject*>(selectedItem);
        ConnectorObject* const selectedConnector = dynamic_cast<ConnectorObject*>(selectedItem);

        selectedItem = nullptr;

        {
            BlockObject* block = findBlockForMousePress(mappedPos);

            if (block != selectedBlock && selectedBlock != nullptr)
            {
                selectedBlock->setSelected(false);
                update();
            }

            if (block != nullptr)
            {
                if (selectedConnector != nullptr)
                {
                    selectedConnector->setSelected(false);
                }

                selectedItem = block;
                selectedItem->setSelected(true);

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
                        block->sceneBoundingRect().center() - mappedPos,
                        snapMousePositionToGrid(block->pos().toPoint()));
                }
            }
        }

        // Check for a selected connector
        if (selectedItem == nullptr)
        {
            ConnectorObject* foundConnector = findConnectorForMousePress(mappedPos);

            if (foundConnector != selectedConnector && selectedConnector != nullptr)
            {
                selectedConnector->setSelected(false);
            }

            if (foundConnector != nullptr)
            {
                foundConnector->setSelected(true);
                selectedItem = foundConnector;
            }
        }
    }
}

void BlockGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (!isEnabled())
    {
        return;
    }

    if (auto* mouseDragState = dynamic_cast<BlockDragState*>(mouseState.get()); mouseDragState != nullptr)
    {
        const QPointF newBlockPos = mapToScene(event->pos()) - mouseDragState->getBlock()->boundingRect().center() + mouseDragState->getOffset();
        const QPoint newBlockPosInt = snapMousePositionToGrid(newBlockPos.toPoint());

        if (newBlockPosInt != mouseDragState->getCurrent())
        {
            mouseDragState->getBlock()->setPos(newBlockPosInt);
            mouseDragState->setCurrent(newBlockPosInt);
            emit modelChanged();
        }
    }
}

void BlockGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (!isEnabled())
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
            const auto conn = std::make_shared<tmdl::Connection>(
                portDragState->get_output().block->get_block()->get_id(),
                portDragState->get_output().port_count,
                portDragState->get_input().block->get_block()->get_id(),
                portDragState->get_input().port_count);

            try
            {
                model->add_connection(conn);
            }
            catch (const tmdl::ModelException& ex)
            {
                QMessageBox::warning(this, "error", ex.what().c_str());
                mouseState = nullptr;
                return;
            }

            addConnectionItem(
                conn,
                portDragState->get_output().block,
                portDragState->get_input().block);

            const auto& items = scene()->items();
            for (auto* i : qAsConst(items))
            {
                auto* blk = dynamic_cast<BlockObject*>(i);
                if (blk != nullptr)
                {
                    blk->update();
                }
            }

            updateModel();
        }
    }

    mouseState = nullptr;
}

void BlockGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (!isEnabled())
    {
        return;
    }

    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        const auto mappedPos = mapToScene(event->pos());
        ;

        if (BlockObject* block = findBlockForMousePress(mappedPos); block != nullptr)
        {
            BlockParameterDialog* dialog = new BlockParameterDialog(block, this);

            dialog->exec();
            updateModel();

            const auto sceneItems = scene()->items();

            for (auto ptr : qAsConst(sceneItems))
            {
                const auto c = dynamic_cast<ConnectorObject*>(ptr);
                if (c == nullptr) continue;

                if (!c->isValidConnection())
                {
                    model->remove_connection(
                        c->get_to_block()->get_block()->get_id(),
                        c->get_to_port());
                    c->deleteLater();
                }
            }

            emit block->sceneLocationUpdated();

            updateModel();
        }
        else if (ConnectorObject* conn = findConnectorForMousePress(mappedPos); conn != nullptr)
        {
            auto dialog = new ConnectionParametersDialog(conn, this);
            if (dialog->exec())
            {
                conn->update();
                emit modelChanged();
            }
        }
    }
}

void BlockGraphicsView::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::EnabledChange)
    {
        if (selectedItem != nullptr)
        {
            selectedItem->setSelected(false);
            selectedItem = nullptr;
            mouseState = nullptr;
        }

        if (isEnabled())
        {
            scene()->setForegroundBrush(Qt::transparent);
        }
        else
        {
            scene()->setForegroundBrush(QColor(100, 100, 100, 100));
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
    if (!isEnabled())
    {
        return;
    }

    if (selectedItem != nullptr)
    {
        if (auto* selectedBlock = dynamic_cast<BlockObject*>(selectedItem); selectedBlock != nullptr)
        {
            model->remove_block(selectedBlock->get_block()->get_id());
        }
        else if (auto* selectedConnector = dynamic_cast<ConnectorObject*>(selectedItem); selectedConnector != nullptr)
        {
            model->remove_connection(selectedConnector->get_to_block()->get_block()->get_id(), selectedConnector->get_to_port());
        }

        scene()->removeItem(selectedItem);
        selectedItem->deleteLater();
        selectedItem = nullptr;

        updateModel();
        emit modelChanged();
    }
}

void BlockGraphicsView::updateModel()
{
    if (!isEnabled())
    {
        return;
    }

    if (model->update_block())
    {
        emit modelChanged();
    }

    const auto sceneItems = scene()->items();
    for (auto* item : qAsConst(sceneItems))
    {
        auto* block = dynamic_cast<BlockObject*>(item);
        if (block != nullptr)
        {
            block->update();
        }
    }

    scene()->update();
    update();

    emit modelUpdated();
}

BlockObject* BlockGraphicsView::findBlockForMousePress(const QPointF& pos)
{
    const auto sceneItems = scene()->items();
    for (auto itm : qAsConst(sceneItems))
    {
        BlockObject* blk = dynamic_cast<BlockObject*>(itm);
        if (blk != nullptr && blk->sceneBoundingRect().contains(pos))
        {
            return blk;
        }
    }

    return nullptr;
}

ConnectorObject* BlockGraphicsView::findConnectorForMousePress(const QPointF& pos)
{
    const auto sceneItems = scene()->items();
    for (auto itm : qAsConst(sceneItems))
    {
        ConnectorObject* conn = dynamic_cast<ConnectorObject*>(itm);
        if (conn != nullptr && conn->positionOnLine(conn->mapFromScene(pos)))
        {
            return conn;
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

void BlockGraphicsView::addConnectionItem(
    const std::shared_ptr<tmdl::Connection> connection,
    const BlockObject* from_block,
    const BlockObject* to_block)
{
    // Construct the connector object
    ConnectorObject* conn_obj = new ConnectorObject(
        connection,
        from_block,
        to_block);
    conn_obj->blockLocationUpdated();

    // Connect up location and destroyed items
    connect(
        from_block,
        &BlockObject::sceneLocationUpdated,
        conn_obj,
        &ConnectorObject::blockLocationUpdated);
    connect(
        to_block,
        &BlockObject::sceneLocationUpdated,
        conn_obj,
        &ConnectorObject::blockLocationUpdated);

    connect(
        from_block,
        &BlockObject::destroyed,
        conn_obj,
        &ConnectorObject::deleteLater);
    connect(
        to_block,
        &BlockObject::destroyed,
        conn_obj,
        &ConnectorObject::deleteLater);

    scene()->addItem(conn_obj);
}

std::shared_ptr<tmdl::Model> BlockGraphicsView::get_model() const
{
    return model;
}

void BlockGraphicsView::set_model(std::shared_ptr<tmdl::Model> mdl)
{
    if (!isEnabled())
    {
        throw 1;
    }

    // Reset the state
    mouseState = nullptr;
    selectedItem = nullptr;
    model = nullptr;
    scene()->clear();

    // Save the model
    model = mdl;

    // Add new block objects
    for (const auto& blk : model->get_blocks())
    {
        // Create the block object
        BlockObject* block_obj = new BlockObject(blk);

        // Add the block to storage/tracking
        scene()->addItem(block_obj);
    }

    // Add new connection objects
    const auto& cm = model->get_connection_manager();

    for (const auto& conn : cm.get_connections())
    {
        // Get the from/to block objects
        BlockObject* from_block = nullptr;
        BlockObject* to_block = nullptr;

        const auto& items = scene()->items();
        for (auto it = items.begin(); it != items.end() && (from_block == nullptr || to_block == nullptr); ++it)
        {
            BlockObject* tmp = dynamic_cast<BlockObject*>(*it);
            if (tmp == nullptr) continue;

            if (tmp->get_block()->get_id() == conn->get_from_id())
            {
                if (from_block != nullptr) throw 1;
                from_block = tmp;
            }

            if (tmp->get_block()->get_id() == conn->get_to_id())
            {
                if (to_block != nullptr) throw 1;
                to_block = tmp;
            }
        }

        if (from_block == nullptr || to_block == nullptr) throw 2;

        // Construct the connector object
        addConnectionItem(cm.get_connection_to(conn->get_to_id(), conn->get_to_port()), from_block, to_block);
    }

    // Update the model
    updateModel();
}

void BlockGraphicsView::addBlock(std::shared_ptr<tmdl::BlockInterface> blk)
{
    if (!isEnabled())
    {
        return;
    }

    // Add the block to the model
    try
    {
        model->add_block(blk);
    }
    catch (const tmdl::ModelException& ex)
    {
        QMessageBox::warning(this, "error", ex.what().c_str());
        return;
    }

    // Create the block object
    BlockObject* block_obj = new BlockObject(blk);
    block_obj->setPos(mapToScene(QPoint(50, 50)));

    // Add the block to storage/tracking
    scene()->addItem(block_obj);

    // State the model is updated
    updateModel();
}
