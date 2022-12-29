// SPDX-License-Identifier: GPL-3.0-only

#include "model_window.h"
#include "./ui_model_window.h"

#include <array>

#include <QIcon>
#include <QPixmap>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

#include "dialogs/model_parameters_dialog.h"

#include <tmdl/model_exception.hpp>
#include <tmdl/library_manager.hpp>

#include <fmt/format.h>


ModelWindow::ModelWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModelWindow),
    changeFlag(false)
{
    // Setup the main UI
    ui->setupUi(this);
    connect(ui->block_graphics, &BlockGraphicsView::modelChanged, [this]() { changeFlag = true; updateTitle(); });
    connect(this, &ModelWindow::modelChanged, [this]() { changeFlag = true; updateTitle(); });

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

void ModelWindow::newModel()
{
    auto* window = new ModelWindow();
    window->show();
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

        nlohmann::json j;
        j["model"] = *ui->block_graphics->get_model();

        std::ostringstream oss;
        oss << std::setw(4) << j;

        file.write(QString(oss.str().c_str()).toUtf8());
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

        {
            std::istringstream iss(data.toStdString());
            nlohmann::json j;
            iss >> j;

            std::shared_ptr<tmdl::Model> mdl = std::make_shared<tmdl::Model>("tmp");
            tmdl::from_json(j["model"], *mdl);

            const auto mdl_library = tmdl::LibraryManager::get_instance().default_model_library();

            if (mdl_library->has_block(mdl->get_name()))
            {
                mdl = mdl_library->get_model(mdl->get_name());
            }
            else
            {
                mdl_library->add_model(mdl);
            }

            changeModel(mdl);
            mdl_library->close_empty_models();
        }

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

void ModelWindow::closeModel()
{
    const auto name = ui->block_graphics->get_model()->get_name();
    const auto weak_val = std::weak_ptr<tmdl::Model>(ui->block_graphics->get_model());

    changeModel(nullptr);

    auto mdl_library = tmdl::LibraryManager::get_instance().default_model_library();

    try
    {
        mdl_library->close_model(name);
    }
    catch (const tmdl::ModelException& ex)
    {
        QMessageBox::warning(this, "error", ex.what().c_str());

        auto model_ptr = weak_val.lock();
        if (model_ptr && mdl_library->has_block(name))
        {
            changeModel(model_ptr);
        }

        return;
    }

    mdl_library->close_empty_models();

    close();
}

void ModelWindow::changeModel(std::shared_ptr<tmdl::Model> model)
{
    if (changeFlag)
    {
        const QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Close?", "Model has unsaved changes - confirm close?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
        {
            return;
        }
    }

    // Close the model
    const auto last_name = ui->block_graphics->get_model()->get_name();

    if (model == nullptr || model->get_name() != last_name)
    {
        // Clear the change flag and load
        changeFlag = false;
        executor = nullptr;

        // Set the new model
        if (model == nullptr)
        {
            model = std::make_shared<tmdl::Model>("tmp");
        }

        ui->block_graphics->setEnabled(true);
        ui->block_graphics->set_model(model);
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

        for (size_t i = 0; i < model->get_num_outputs(); ++i)
        {
            const auto outer_id = tmdl::VariableIdentifier {
                .block_id = 0, // TODO get_id(),
                .output_port_num = i
            };

            executor->variables->set_name_for_variable(fmt::format("*Output {} ({})", i, outer_id.to_string()), outer_id);
        }

        updateMenuBars();
    }
    catch (const tmdl::ModelException& ex)
    {
        QMessageBox::warning(this, "Parameter Error", ex.what().c_str());
        executor = nullptr;
        return;
    }

    emit executorEvent(SimEvent(SimEvent::EventType::Create));
}

void ModelWindow::stepExecutor()
{
    if (executor == nullptr)
    {
        return;
    }

    if (executor->iterations != 0)
    {
        executor->state.set_time(executor->state.get_time() + executor->state.get_dt());
    }
    executor->iterations += 1;
    executor->model->step(executor->state);

    emit executorEvent(SimEvent(SimEvent::EventType::Step));
}

void ModelWindow::clearExecutor()
{
    if (executor != nullptr)
    {
        executor = nullptr;
        updateMenuBars();
        emit executorEvent(SimEvent(SimEvent::EventType::Close));
    }

    if (window_plot != nullptr)
    {
        window_plot->close();
        window_plot = nullptr;
    }
}

void ModelWindow::resetExecutor()
{
    if (executor != nullptr)
    {
        executor->reset();
        emit executorEvent(SimEvent(SimEvent::EventType::Reset));
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
    if (window_parameters->exec())
    {
        emit modelChanged();
    }
}

void ModelWindow::showPlot()
{
    if (window_plot == nullptr)
    {
        window_plot = new PlotWindow(executor);

        connect(this, &ModelWindow::executorEvent, window_plot, &PlotWindow::executorEvent);
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

    tmdl::LibraryManager::get_instance().default_model_library()->close_empty_models();
}
