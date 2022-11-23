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

#include <tmdl/stdlib/tmdl_stdlib.hpp>

#include "blocks/connector_object.h"

#include "state/block_drag_state.h"
#include "state/port_drag_state.h"

#include "windows/parameter_dialog.h"
#include "windows/plot_window.h"


BlockGraphicsView::BlockGraphicsView(QWidget* parent) :
    QGraphicsView(parent),
    selectedBlock(nullptr)
{
    // Set the scene
    setScene(new QGraphicsScene(this));
    setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Set the library
    library = std::make_shared<tmdl::stdlib::StandardLibrary>();
    libraryWindow = nullptr;
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

            const auto sceneItems = scene()->items();

            for (auto ptr : qAsConst(sceneItems))
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

            for (auto ptr : qAsConst(sceneItems))
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

void BlockGraphicsView::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_S:
        stepExecutor();
        break;
    }
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
        scene()->update();
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
    }
}

void BlockGraphicsView::generateExecutor()
{
    tmdl::ConnectionManager connections;
    std::shared_ptr<tmdl::VariableManager> manager = std::make_shared<tmdl::VariableManager>();

    try
    {
        // Add each output variable to the manager
        for (size_t i = 0; i < model.get_num_outputs(); ++i)
        {
            const auto pv = model.get_output_port(i);

            std::shared_ptr<tmdl::ValueBox> value;

            switch (pv.dtype)
            {
            case tmdl::DataType::BOOLEAN:
                value = std::make_shared<tmdl::ValueBoxType<bool>>(false);
                break;
            case tmdl::DataType::SINGLE:
                value = std::make_shared<tmdl::ValueBoxType<float>>(0.0f);
                break;
            case tmdl::DataType::DOUBLE:
                value = std::make_shared<tmdl::ValueBoxType<double>>(0.0);
                break;
            case tmdl::DataType::INT32:
                value = std::make_shared<tmdl::ValueBoxType<int32_t>>(0);
                break;
            case tmdl::DataType::UINT32:
                value = std::make_shared<tmdl::ValueBoxType<uint32_t>>(0);
                break;
            default:
                throw tmdl::ModelException("unable to construct value for type");
            }

            const auto vid = tmdl::VariableIdentifier
            {
                .block_id = model.get_id(),
                .output_port_num = i
            };

            manager->add_variable(vid, value);
        }

        model.update_block();

        executor = std::make_unique<ExecutionState>(ExecutionState
        {
            .variables = manager,
            .model = model.get_execution_interface(
                connections,
                *manager),
            .state = tmdl::SimState
            {
                .time = 0.0,
                .dt = 0.1
            }
        });
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

    if (libraryWindow != nullptr)
    {
        libraryWindow->close();
    }
}

void BlockGraphicsView::stepExecutor()
{
    if (executor == nullptr)
    {
        return;
    }

    const auto vid = tmdl::VariableIdentifier
    {
        .block_id = model.get_id(),
        .output_port_num = 0
    };

    auto var = std::dynamic_pointer_cast<tmdl::ValueBoxType<double>>(executor->variables->get_ptr(vid));

    if (executor->state.time < executor->state.dt && var)
    {
        emit plotPointUpdated(executor->state.time, var->value);
    }

    executor->state.time += executor->state.dt;
    executor->model->step(executor->state);

    if (var)
    {
        emit plotPointUpdated(executor->state.time, var->value);
    }
}

void BlockGraphicsView::clearExecutor()
{
    executor = nullptr;
}

void BlockGraphicsView::showLibrary()
{
    libraryWindow = new BlockLibrary(this);
    libraryWindow->set_library(library);

    connect(
        libraryWindow,
        &BlockLibrary::blockSelected,
        this,
        &BlockGraphicsView::addBlock);

    connect(
        libraryWindow,
        &BlockLibrary::destroyed,
        this,
        &BlockGraphicsView::libraryClosed);

    libraryWindow->show();
}

void BlockGraphicsView::showPlot()
{
    auto* plt = new PlotWindow(this);

    connect(this, &BlockGraphicsView::plotPointUpdated, plt, &PlotWindow::addPlotPoint);

    plt->show();
}

void BlockGraphicsView::libraryClosed()
{
    libraryWindow = nullptr;
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
