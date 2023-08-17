// SPDX-License-Identifier: GPL-3.0-only

#include "model_window.h"
#include "./ui_model_window.h"

#include <array>
#include <filesystem>
#include <fstream>

#include <QIcon>
#include <QPixmap>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

#include "dialogs/model_parameters_dialog.h"

#include <tmdl/model_exception.hpp>
#include <tmdl/library_manager.hpp>
#include <tmdl/model_block.hpp>

#include <tmdl/codegen/generator.hpp>

#include <fmt/format.h>

#include "../managers/executor_manager.h"
#include "../managers/window_manager.h"

#include "exceptions/model_exception.h"

const QString ModelWindow::default_file_filter = QString("Model (*%1);; Any (*.*)").arg(tmdl::Model::DEFAULT_MODEL_EXTENSION.c_str());


ModelWindow::ModelWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModelWindow),
    changeFlag(false)
{
    // Setup the main UI
    ui->setupUi(this);
    connect(ui->block_graphics, &BlockGraphicsView::modelChanged, this, &ModelWindow::setChangedFlag);
    connect(this, &ModelWindow::modelChanged, this, &ModelWindow::setChangedFlag);

    // Connect the manager parameter
    connect(
        &ExecutorManager::instance(), &ExecutorManager::executorFlagChanged,
        this, &ModelWindow::executorFlagSet);

    // Add to window manager
    WindowManager::instance().register_window(this, get_model_id());

    // Update the menu items
    updateWindowItems();
}

ModelWindow::~ModelWindow()
{
    delete ui;
    tmdl::LibraryManager::get_instance().default_model_library()->close_unused_models();
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

    clearExecutor();

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
    
    WindowManager::instance().clear_window(this);
    ui->block_graphics->set_model(nullptr);

    deleteLater();
}


void ModelWindow::keyPressEvent(QKeyEvent* event)
{
    bool handled = true;

    if (event->modifiers() == Qt::NoModifier)
    {
        switch (event->key())
        {
        case Qt::Key_S:
            stepExecutor();
            break;
        case Qt::Key_Delete:
            ui->block_graphics->removeSelectedBlock();
            break;
        default:
            handled = false;
        }
    }
    else if (event->modifiers() == Qt::ControlModifier)
    {
        switch (event->key())
        {
        case Qt::Key_L:
            showLibrary();
            break;
        case Qt::Key_D:
            showDiagnostics();
            break;
        case Qt::Key_E:
            showModelParameters();
            break;
        default:
            handled = false;
        }
    }

    if (!handled)
    {
        QMainWindow::keyPressEvent(event);
    }
}

void ModelWindow::updateWindowItems()
{
    const bool generatedAvailable = executor != nullptr;

    const auto executorWindow = ExecutorManager::instance().windowExecutor();
    const bool otherExecutorExists = executorWindow.has_value() && *executorWindow != get_model_id();

    ui->menuModel->setEnabled(!generatedAvailable);
    ui->menuBlocks->setEnabled(!generatedAvailable);
    ui->menuSim->setEnabled(generatedAvailable);

    ui->block_graphics->setEnabled(!generatedAvailable);

    QString windowTitle = QString("%1%2")
        .arg(currentModelName())
        .arg(changeFlag ? "*" : "");
    setWindowTitle(windowTitle);

    setEnabled(!otherExecutorExists);
}

void ModelWindow::setChangedFlag()
{
    changeFlag = true;
    updateWindowItems();
}

void ModelWindow::newModel()
{
    auto* window = new ModelWindow();
    window->show();
}

void ModelWindow::saveModel()
{
    if (!get_model_id()->get_filename().has_value())
    {
        saveModelAs();
    }
    else
    {
        try
        {
            get_model_id()->save_model();
        }
        catch (const tmdl::ModelException& ex)
        {
            QMessageBox::warning(this, "error", ex.what());
            return;
        }

        changeFlag = false;
        updateWindowItems();
    }
}

void ModelWindow::saveModelAs()
{
    QString saveName = QFileDialog::getSaveFileName(this, tr("Save Model"), get_filename(), tr(default_file_filter.toUtf8()));
    if (!saveName.isEmpty())
    {
        std::filesystem::path pth(saveName.toStdString());
        if (!pth.has_extension())
        {
            pth.replace_extension(tmdl::Model::DEFAULT_MODEL_EXTENSION);
        }

        try
        {
            get_model_id()->save_model_to(pth);
        }
        catch (const tmdl::ModelException& ex)
        {
            QMessageBox::warning(this, "error", ex.what());
            return;
        }

        changeFlag = false;
        updateWindowItems();
    }
}

void ModelWindow::openFileDialog()
{
    QString openName = QFileDialog::getOpenFileName(this, tr("Open Model"), get_filename(), tr(default_file_filter.toUtf8()));
    if (!openName.isEmpty())
    {
        openModelFile(openName);
    }
}

bool ModelWindow::openModelFile(QString openFilename)
{
    std::shared_ptr<tmdl::Model> mdl = nullptr;

    try
    {
        mdl = tmdl::Model::load_model(openFilename.toStdString());
    }
    catch (const tmdl::ModelException& ex)
    {
        QMessageBox::warning(this, "error", ex.what());
        return false;
    }

    if (mdl == nullptr || !openModel(mdl))
    {
        return false;
    }

    return true;
}

bool ModelWindow::openModel(std::shared_ptr<tmdl::Model> model)
{
    const auto mdl_library = tmdl::LibraryManager::get_instance().default_model_library();

    if (auto mdl_new = mdl_library->try_get_model(model->get_name()))
    {
        model = mdl_new;
    }
    else
    {
        mdl_library->add_model(model);
    }

    if (WindowManager::instance().model_is_open(model.get()))
    {
        QMessageBox::warning(this, "error", fmt::format("Model `{}` is already open", model->get_name()).c_str());
        return false;
    }

    changeModel(model);
    mdl_library->close_unused_models();

    changeFlag = false;
    updateWindowItems();

    if (window_diagnostics != nullptr)
    {
        window_diagnostics->setModel(ui->block_graphics->get_model());
    }

    if (window_library != nullptr)
    {
        window_library->updateLibrary();
    }

    return true;
}

void ModelWindow::closeModel()
{
    const auto model = ui->block_graphics->get_model();
    const auto name = model->get_name();
    const auto weak_val = std::weak_ptr<tmdl::Model>(model);
    const auto model_id = model.get();

    changeModel(nullptr);

    auto mdl_library = tmdl::LibraryManager::get_instance().default_model_library();

    try
    {
        mdl_library->close_model(model_id);
    }
    catch (const tmdl::ModelException& ex)
    {
        QMessageBox::warning(this, "error", ex.what());

        auto model_ptr = weak_val.lock();
        if (model_ptr && mdl_library->has_block(name))
        {
            changeModel(model_ptr);
        }

        return;
    }
    
    mdl_library->close_unused_models();

    close();
}

void ModelWindow::saveCode()
{
    const auto fn = get_filename();
    if (fn.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Must save file before generating code");
        return;
    }

    try
    {
        const auto model = ui->block_graphics->get_model();
        tmdl::codegen::CodeGenerator gen(ui->block_graphics->get_block()->get_compiled(tmdl::BlockInterface::ModelInfo(model->get_preferred_dt())));

        std::filesystem::path gen_path(fn.toStdString());
        gen.write_in_folder(gen_path.parent_path());
    }
    catch (const tmdl::codegen::CodegenError& err)
    {
        QMessageBox::warning(this, "Error", err.what());
    }
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
    const auto last_name = get_model_id()->get_name();

    if (model == nullptr || model->get_name() != last_name)
    {
        // Clear the change flag and load
        changeFlag = false;
        executor = nullptr;

        // Set the new model
        if (model == nullptr)
        {
            model = std::make_shared<tmdl::Model>();
        }
        else if (WindowManager::instance().model_is_open(model.get()))
        {
            throw ModelException("model already open");
        }

        ui->block_graphics->setEnabled(true);
        ui->block_graphics->set_model(model);

        WindowManager::instance().register_window(this, get_model_id());
    }
}

void ModelWindow::showDiagnostics()
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
        executor = std::make_shared<tmdl::ExecutionState>(tmdl::ExecutionState::from_model(model, model->get_preferred_dt()));
        executor->init();

        for (size_t i = 0; i < model->get_num_outputs(); ++i)
        {
            const auto outer_id = tmdl::VariableIdentifier {
                .block_id = 0, // TODO get_id(),
                .output_port_num = i
            };

            const std::string varname = fmt::format("*Output {} ({})", i, model->get_output_ids()[i]);
            executor->add_name_to_variable(varname, outer_id);
        }

        for (auto& c : model->get_connection_manager().get_connections())
        {
            const auto& varname = c->get_name();

            if (varname.empty())
            {
                continue;
            }

            executor->add_name_to_interior_variable(varname, *c);
        }

        ExecutorManager::instance().setWindowExecutor(get_model_id());

        updateWindowItems();
    }
    catch (const tmdl::ModelException& ex)
    {
        QMessageBox::warning(this, "Parameter Error", ex.what());
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

    executor->step();

    emit executorEvent(SimEvent(SimEvent::EventType::Step));
}

void ModelWindow::resetExecutor()
{
    if (executor != nullptr)
    {
        executor->reset();
        emit executorEvent(SimEvent(SimEvent::EventType::Reset));
    }
}

void ModelWindow::clearExecutor()
{
    if (executor != nullptr)
    {
        executor = nullptr;
        updateWindowItems();
        emit executorEvent(SimEvent(SimEvent::EventType::Close));
    }

    if (window_plot != nullptr)
    {
        window_plot->close();
        window_plot = nullptr;
    }

    ExecutorManager::instance().reset();
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
    auto* window_parameters = new ModelParametersDialog(ui->block_graphics->get_model(), currentModelName(), this);
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

    // Initialze the block
    const auto tmp = tmdl::LibraryManager::get_instance().get_library(l.toStdString())->create_block(s.toStdString());

    // Add the block
    ui->block_graphics->addBlock(tmp);
}

void ModelWindow::executorFlagSet()
{
    updateWindowItems();
}

QString ModelWindow::currentModelName() const
{
    auto model_name = ui->block_graphics->get_model()->get_name();
    if (model_name.empty())
    {
        model_name = "UNTITLED_MODEL";
    }
    return QString(model_name.c_str());
}

tmdl::Model* ModelWindow::get_model_id()
{
    return ui->block_graphics->get_model().get();
}

const tmdl::Model* ModelWindow::get_model_id() const
{
    return ui->block_graphics->get_model().get();
}

QString ModelWindow::get_filename() const
{
    if (get_model_id() == nullptr)
    {
        return "";
    }

    const auto tmp_filename = get_model_id()->get_filename();

    if (tmp_filename.has_value())
    {
        return tmp_filename->c_str();
    }
    else
    {
        return "";
    }
}
