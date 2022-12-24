// SPDX-License-Identifier: GPL-3.0-only

#include "model_window.h"
#include "./ui_model_window.h"

#include <array>

#include <QIcon>
#include <QPixmap>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

#include "dialogs/block_parameters_dialog.h"
#include "dialogs/model_parameters_dialog.h"

#include <tmdl/model_exception.hpp>
#include <tmdl/library_manager.hpp>


ModelWindow::ModelWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModelWindow),
    changeFlag(false)
{
    // Setup the main UI
    ui->setupUi(this);
    connect(ui->block_graphics, &BlockGraphicsView::modelChanged, [this]() { changeFlag = true; updateTitle(); });

    // Update the menu items
    updateMenuBars();
    updateTitle();
}

void ModelWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_S:
        stepExecutor();
        break;
    default:
        event->ignore();
    }
}

void ModelWindow::updateMenuBars()
{
    const bool generatedAvailable = executor != nullptr;

    ui->menuModel->setEnabled(!generatedAvailable);
    ui->menuBlocks->setEnabled(!generatedAvailable);
    ui->menuSim->setEnabled(generatedAvailable);

    ui->block_graphics->setEnabled(!generatedAvailable);
}

void ModelWindow::updateTitle()
{
    QString windowTitle = QString("%1%2")
        .arg(ui->block_graphics->get_model()->get_name().c_str())
        .arg(changeFlag ? "*" : "");
    setWindowTitle(windowTitle);
}

void ModelWindow::saveModel()
{
    if (filename.isEmpty())
    {
        saveModelAs();
    }
    else
    {
        QFile file(filename);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this, "error", file.errorString());
            return;
        }

        const auto s = ui->block_graphics->getJsonString();
        file.write(QString(s.c_str()).toUtf8());
        file.close();
        changeFlag = false;
        updateTitle();
    }
}

void ModelWindow::saveModelAs()
{
    QString saveName = QFileDialog::getSaveFileName(this, tr("Save Model"), filename, "JSON (*.json); Any (*.*)");
    if (!saveName.isEmpty())
    {
        filename = saveName;
        saveModel();
    }
}

void ModelWindow::openModel()
{
    QString openName = QFileDialog::getOpenFileName(this, tr("Open Model"), filename, "JSON (*.json); Any (*.*)");
    if (!openName.isEmpty())
    {
        QFile file(openName);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, "error", file.errorString());
            return;
        }

        QTextStream stream(&file);
        const QString data = stream.readAll();
        file.close();

        ui->block_graphics->fromJsonString(data.toStdString());

        if (window_library != nullptr)
        {
            window_library->updateLibrary();
        }

        filename = openName;
        changeFlag = false;
        updateTitle();

        if (window_diagnostics != nullptr)
        {
            window_diagnostics->setModel(ui->block_graphics->get_model());
        }
    }
}

void ModelWindow::showErrors()
{
    if (window_diagnostics == nullptr)
    {
        window_diagnostics = new ModelDiagnosticsDialog(ui->block_graphics->get_model());

        connect(window_diagnostics, &ModelDiagnosticsDialog::destroyed, [this]() {
            window_diagnostics = nullptr;
        });

        connect(ui->block_graphics, &BlockGraphicsView::modelUpdated, window_diagnostics, &ModelDiagnosticsDialog::updateDiagnostics);
    }

    window_diagnostics->show();
}

void ModelWindow::generateExecutor()
{
    if (window_library != nullptr)
    {
        window_library->close();
        window_library = nullptr;
    }

    ui->block_graphics->updateModel();
    const auto model = ui->block_graphics->get_model();

    try
    {
        executor = std::make_shared<tmdl::ExecutionState>(tmdl::ExecutionState::from_model(model, 0.1));
        updateMenuBars();
    }
    catch (const tmdl::ModelException& ex)
    {
        QMessageBox::warning(this, "Parameter Error", ex.what().c_str());
        executor = nullptr;
        return;
    }
}

static std::optional<double> double_from_variable(std::shared_ptr<const tmdl::ValueBox> ptr)
{
    if (auto v = std::dynamic_pointer_cast<const tmdl::ValueBoxType<double>>(ptr); v)
    {
        return v->value;
    }
    else if (auto v = std::dynamic_pointer_cast<const tmdl::ValueBoxType<float>>(ptr); v)
    {
        return static_cast<double>(v->value);
    }
    else if (auto v = std::dynamic_pointer_cast<const tmdl::ValueBoxType<int32_t>>(ptr); v)
    {
        return static_cast<double>(v->value);
    }
    else if (auto v = std::dynamic_pointer_cast<const tmdl::ValueBoxType<uint32_t>>(ptr); v)
    {
        return static_cast<double>(v->value);
    }
    else if (auto v = std::dynamic_pointer_cast<const tmdl::ValueBoxType<bool>>(ptr); v)
    {
        return (v->value) ? 1.0 : 0.0;
    }
    else
    {
        return std::nullopt;
    }
}

void ModelWindow::stepExecutor()
{
    if (executor == nullptr)
    {
        return;
    }

    const auto vid = tmdl::VariableIdentifier
    {
        .block_id = /* TODO: model.get_id() */ 0,
        .output_port_num = 0
    };

    auto var = executor->variables->get_ptr(vid);

    if (executor->iterations != 0)
    {
        executor->state.set_time(executor->state.get_time() + executor->state.get_dt());
    }
    executor->iterations += 1;
    executor->model->step(executor->state);

    auto double_val = double_from_variable(var);
    if (double_val)
    {
        emit plotPointUpdated(executor->state.get_time(), double_val.value());
    }

    emit executorStepped();
}

void ModelWindow::clearExecutor()
{
    if (executor != nullptr)
    {
        executor = nullptr;
        updateMenuBars();
        emit executorDestroyed();
    }
}

void ModelWindow::resetExecutor()
{
    if (executor != nullptr)
    {
        executor->reset();
        emit executorReset();
    }
}

void ModelWindow::showLibrary()
{
    if (window_library == nullptr)
    {
        window_library = new BlockSelectorDialog(this);

        connect(
            window_library,
            &BlockSelectorDialog::blockSelected,
            this,
            &ModelWindow::addBlock);

        connect(window_library, &BlockSelectorDialog::destroyed, [this]() { window_library = nullptr; });
    }

    window_library->show();
}

void ModelWindow::showModelParameters()
{
    auto* window_parameters = new ModelParametersDialog(ui->block_graphics->get_model(), this);
    window_parameters->exec();
}

void ModelWindow::showPlot()
{
    if (window_plot == nullptr)
    {
        window_plot = new PlotWindow(executor);

        connect(this, &ModelWindow::plotPointUpdated, window_plot, &PlotWindow::addPlotPoint);
        connect(window_plot, &PlotWindow::destroyed, [this]() { window_plot = nullptr; });
    }

    window_plot->show();
}

void ModelWindow::addBlock(QString l, QString s)
{
    if (executor != nullptr)
    {
        return;
    }

    // Ensure that the block isn't the current block
    const auto model = ui->block_graphics->get_model();
    if (s.toStdString() == model->get_name())
    {
        QMessageBox::warning(this, "Block Addition", "Cannot add recursive element of same block");
        return;
    }

    // Initialze the block
    const auto tmp = tmdl::LibraryManager::get_instance().get_library(l.toStdString())->create_block(s.toStdString());
    ui->block_graphics->addBlock(tmp);
}

ModelWindow::~ModelWindow()
{
    delete ui;
}

void ModelWindow::closeEvent(QCloseEvent* event)
{
    if (changeFlag)
    {
        const QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Quit?", "Model has unsaved changes - confirm exit?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
        {
            event->ignore();
            return;
        }
    }

    std::array<QWidget*, 3> windows = {
        window_diagnostics,
        window_library,
        window_plot
    };

    for (auto& w : windows)
    {
        if (w != nullptr)
        {
            w->close();
        }
    }

    window_diagnostics = nullptr;
    window_library = nullptr;
    window_plot = nullptr;
}
