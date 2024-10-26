// SPDX-License-Identifier: GPL-3.0-only

#include "model_window.h"
#include "./ui_model_window.h"

#include <array>
#include <filesystem>

#include <QFileDialog>
#include <QIcon>
#include <QMessageBox>
#include <QPixmap>
#include <QTextStream>

#include "dialogs/model_parameters_dialog.h"

#include <model_manager.hpp>
#include <model_block.hpp>
#include <model_exception.hpp>

#include <codegen_generator.hpp>

#include <fmt/format.h>

#include "../managers/executor_manager.h"
#include "../managers/window_manager.h"

#include "exceptions/model_exception.h"

const QString ModelWindow::default_file_filter = QString("Model (*%1);; Any (*.*)").arg(mtea::Model::DEFAULT_MODEL_EXTENSION.c_str());

ModelWindow::ModelWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::ModelWindow) {
    // Setup the main UI
    ui->setupUi(this);
    connect(ui->block_graphics, &BlockGraphicsView::modelChanged, this, &ModelWindow::setChangedFlag);
    connect(this, &ModelWindow::modelChanged, this, &ModelWindow::setChangedFlag);

    // Connect the manager parameter
    connect(&ExecutorManager::instance(), &ExecutorManager::executorFlagChanged, this, &ModelWindow::executorFlagSet);

    // Add to window manager
    WindowManager::instance().register_window(this, get_model_id());

    // Update the menu items
    updateWindowItems();

    setAttribute(Qt::WA_DeleteOnClose, true);
}

ModelWindow::~ModelWindow() {
    delete ui;
    mtea::ModelManager::get_instance().default_model_library()->close_unused_models();
}

bool ModelWindow::check_can_close_current_model() {
    if (has_unsaved_changes()) {
        const QMessageBox::StandardButton reply =
            QMessageBox::question(this, "Quit?", "Model has unsaved changes - confirm exit?", QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return false;
        }
    }

    return true;
}

void ModelWindow::closeEvent(QCloseEvent* event) {
    std::array<QWidget*, 3> windows = {window_diagnostics, window_library, window_plot};

    for (auto& w : windows) {
        if (w != nullptr) {
            w->close();
        }
    }

    if (!check_can_close_current_model()) {
        event->ignore();
        return;
    }

    clearExecutor();

    WindowManager::instance().clear_window(this);
    ui->block_graphics->set_model(nullptr);

    deleteLater();
}

void ModelWindow::keyPressEvent(QKeyEvent* event) {
    bool handled = true;

    if (event->modifiers() == Qt::NoModifier) {
        switch (event->key()) {
        case Qt::Key_S:
            stepExecutor();
            break;
        case Qt::Key_Delete:
            ui->block_graphics->removeSelectedBlock();
            break;
        default:
            handled = false;
        }
    } else if (event->modifiers() == Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_L:
            showLibrary();
            break;
        case Qt::Key_D:
            showDiagnostics();
            break;
        case Qt::Key_E:
            showModelParameters();
            break;
        case Qt::Key_S:
            saveModel();
            break;
        default:
            handled = false;
        }
    }

    if (!handled) {
        event->ignore();
    }
}

void ModelWindow::updateWindowItems() {
    const bool generatedAvailable = executor != nullptr;

    const auto executorWindow = ExecutorManager::instance().windowExecutor();
    const bool otherExecutorExists = executorWindow.has_value() && *executorWindow != get_model_id();

    ui->menuModel->setEnabled(!generatedAvailable);
    ui->menuBlocks->setEnabled(!generatedAvailable);
    ui->menuSim->setEnabled(generatedAvailable);

    ui->block_graphics->setEnabled(!generatedAvailable);

    QString windowTitle = QString("%1%2").arg(currentModelName()).arg(has_unsaved_changes() ? "*" : "");
    setWindowTitle(windowTitle);

    setEnabled(!otherExecutorExists);
}

void ModelWindow::setChangedFlag() { updateWindowItems(); }

void ModelWindow::newModel() {
    auto* window = new ModelWindow();
    window->show();
}

void ModelWindow::saveModel() {
    if (!get_model_id()->get_filename().has_value()) {
        saveModelAs();
    } else {
        const auto mdl_library = mtea::ModelManager::get_instance().default_model_library();

        try {
            mdl_library->save_model(get_model_id());
        } catch (const mtea::ModelException& ex) {
            QMessageBox::warning(this, "error", ex.what());
            return;
        }

        updateWindowItems();
    }
}

void ModelWindow::saveModelAs() {
    QString saveName = QFileDialog::getSaveFileName(this, tr("Save Model"), get_filename(), tr(default_file_filter.toUtf8()));
    if (!saveName.isEmpty()) {
        std::filesystem::path pth(saveName.toStdString());
        if (!pth.has_extension()) {
            pth.replace_extension(mtea::Model::DEFAULT_MODEL_EXTENSION);
        }

        const auto mdl_library = mtea::ModelManager::get_instance().default_model_library();

        try {
            mdl_library->save_model(get_model_id(), pth);
        } catch (const mtea::ModelException& ex) {
            QMessageBox::warning(this, "error", ex.what());
            return;
        }

        updateWindowItems();
    }
}

void ModelWindow::openFileDialog() {
    QString openName = QFileDialog::getOpenFileName(this, tr("Open Model"), get_filename(), tr(default_file_filter.toUtf8()));
    if (!openName.isEmpty()) {
        if (!get_model_id()->get_filename().has_value()) {
            openModelFile(openName);
        } else {
            auto new_window = new ModelWindow();
            if (new_window->openModelFile(openName)) {
                new_window->show();
            } else {
                new_window->close();
            }
        }
    }
}

bool ModelWindow::openModelFile(QString openFilename) {
    std::shared_ptr<mtea::Model> mdl = nullptr;
    const auto mdl_library = mtea::ModelManager::get_instance().default_model_library();

    try {
        mdl = mdl_library->load_model(openFilename.toStdString());
    } catch (const mtea::ModelException& ex) {
        QMessageBox::warning(this, "error", ex.what());
        return false;
    }

    if (mdl == nullptr || !openModel(mdl)) {
        return false;
    }

    return true;
}

bool ModelWindow::openModel(std::shared_ptr<mtea::Model> model) {
    const auto mdl_library = mtea::ModelManager::get_instance().default_model_library();

    if (auto mdl_new = mdl_library->try_get_model(model->get_name())) {
        model = mdl_new;
    } else {
        (void)mdl_library->add_model(model); // TODO - check where this is loaded and move add model to block library
    }

    if (WindowManager::instance().model_is_open(model.get())) {
        QMessageBox::warning(this, "error", fmt::format("Model `{}` is already open", model->get_name()).c_str());
        return false;
    }

    changeModel(model);
    mdl_library->close_unused_models();
    updateWindowItems();

    if (window_diagnostics != nullptr) {
        window_diagnostics->setModel(ui->block_graphics->get_model());
    }

    if (window_library != nullptr) {
        window_library->updateLibrary();
    }

    return true;
}

void ModelWindow::closeModel() {
    const auto model = ui->block_graphics->get_model();
    const auto name = model->get_name();
    const auto weak_val = std::weak_ptr<mtea::Model>(model);
    const auto model_id = model.get();

    changeModel(nullptr);

    auto mdl_library = mtea::ModelManager::get_instance().default_model_library();

    try {
        if (!name.empty()) {
            mdl_library->close_model(model_id);
        }
    } catch (const mtea::ModelException& ex) {
        QMessageBox::warning(this, "error", ex.what());

        auto model_ptr = weak_val.lock();
        if (model_ptr && mdl_library->has_block(name)) {
            changeModel(model_ptr);
        }

        return;
    }

    mdl_library->close_unused_models();

    close();
}

void ModelWindow::saveCode() {
    const auto fn = get_filename();

    try {
        const auto model = ui->block_graphics->get_model();
        mtea::codegen::CodeGenerator gen(std::make_unique<mtea::ModelBlock>(ui->block_graphics->get_model(), "")
                                             ->get_compiled(mtea::BlockInterface::ModelInfo(model->get_preferred_dt())));

        std::filesystem::path gen_path(fn.toStdString());
        gen.write_in_folder(gen_path.parent_path());
    } catch (const mtea::codegen::CodegenError& err) {
        QMessageBox::warning(this, "Error", err.what());
    }
}

void ModelWindow::exit_all() {
    for (const auto& window : WindowManager::instance().all_windows()) {
        const_cast<ModelWindow*>(window)->close();
    }
}

void ModelWindow::changeModel(std::shared_ptr<mtea::Model> model) {
    if (!check_can_close_current_model()) {
        return;
    }

    // Close the model
    const auto last_name = get_model_id()->get_name();

    if (model == nullptr || model->get_name() != last_name) {
        // Clear the change flag and load
        executor = nullptr;

        // Set the new model
        if (model == nullptr) {
            model = std::make_shared<mtea::Model>();
        } else if (WindowManager::instance().model_is_open(model.get())) {
            throw ModelException("model already open");
        }

        ui->block_graphics->setEnabled(true);
        ui->block_graphics->set_model(model);

        WindowManager::instance().register_window(this, get_model_id());
    }
}

void ModelWindow::showDiagnostics() {
    if (window_diagnostics == nullptr) {
        window_diagnostics = new ModelDiagnosticsDialog(ui->block_graphics->get_model());

        connect(window_diagnostics, &ModelDiagnosticsDialog::finished, [this]() {
            window_diagnostics->deleteLater();
            window_diagnostics = nullptr;
        });

        connect(ui->block_graphics, &BlockGraphicsView::modelUpdated, window_diagnostics, &ModelDiagnosticsDialog::updateDiagnostics);
    }

    window_diagnostics->show();
}

void ModelWindow::generateExecutor() {
    if (window_library != nullptr) {
        window_library->close();
    }

    ui->block_graphics->updateModel();
    const auto model = ui->block_graphics->get_model();

    try {
        executor = std::make_shared<mtea::ExecutionState>(mtea::ExecutionState::from_model(model, model->get_preferred_dt()));
        executor->init();

        for (size_t i = 0; i < model->get_num_outputs(); ++i) {
            const auto outer_id = mtea::VariableIdentifier{.block_id = 0, .output_port_num = i};

            const std::string varname = fmt::format("*Output {} ({})", i, model->get_output_ids()[i]);
            executor->add_name_to_variable(varname, outer_id);
        }

        for (auto& c : model->get_connection_manager().get_connections()) {
            const auto varname = c->get_name();

            if (!varname.has_value()) {
                continue;
            }

            executor->add_name_to_interior_variable(varname->get(), *c);
        }

        ExecutorManager::instance().setWindowExecutor(get_model_id());

        updateWindowItems();
    } catch (const mtea::ModelException& ex) {
        QMessageBox::warning(this, "Parameter Error", ex.what());
        executor = nullptr;
        return;
    }

    emit executorEvent(SimEvent(SimEvent::EventType::Create));
}

void ModelWindow::stepExecutor() {
    if (executor == nullptr) {
        return;
    }

    executor->step();

    emit executorEvent(SimEvent(SimEvent::EventType::Step));
}

void ModelWindow::resetExecutor() {
    if (executor != nullptr) {
        executor->reset();
        emit executorEvent(SimEvent(SimEvent::EventType::Reset));
    }
}

void ModelWindow::clearExecutor() {
    if (executor != nullptr) {
        executor = nullptr;
        updateWindowItems();
        emit executorEvent(SimEvent(SimEvent::EventType::Close));
    }

    if (window_plot != nullptr) {
        window_plot->close();
    }

    ExecutorManager::instance().reset();
}

void ModelWindow::showLibrary() {
    if (window_library == nullptr) {
        window_library = new BlockSelectorDialog(this);

        connect(window_library, &BlockSelectorDialog::blockSelected, this, &ModelWindow::addBlock);
        connect(window_library, &BlockSelectorDialog::finished, [this]() {
            window_library->deleteLater();
            window_library = nullptr;
        });
    }

    window_library->show();
}

void ModelWindow::showModelParameters() {
    auto* window_parameters = new ModelParametersDialog(ui->block_graphics->get_model(), currentModelName(), this);
    connect(window_parameters, &ModelParametersDialog::finished, [this, window_parameters](const int result) {
        if (result) {
            emit modelChanged();
        }
        window_parameters->deleteLater();
    });
    window_parameters->open();
}

void ModelWindow::showPlot() {
    if (window_plot == nullptr) {
        window_plot = new PlotWindow(executor);

        connect(this, &ModelWindow::executorEvent, window_plot, &PlotWindow::executorEvent);
        connect(window_plot, &PlotWindow::destroyed, [this]() { window_plot = nullptr; });
    }

    window_plot->show();
}

void ModelWindow::addBlock(QString l, QString s) {
    if (executor != nullptr) {
        return;
    }

    // Initialze the block
    auto tmp = mtea::ModelManager::get_instance().get_library(l.toStdString())->create_block(s.toStdString());

    // Add the block
    ui->block_graphics->addBlock(std::move(tmp));
}

void ModelWindow::executorFlagSet() { updateWindowItems(); }

QString ModelWindow::currentModelName() const {
    auto model_name = ui->block_graphics->get_model()->get_name();
    if (model_name.empty()) {
        model_name = "UNTITLED_MODEL";
    }
    return QString(model_name.c_str());
}

mtea::Model* ModelWindow::get_model_id() { return ui->block_graphics->get_model().get(); }

const mtea::Model* ModelWindow::get_model_id() const { return ui->block_graphics->get_model().get(); }

QString ModelWindow::get_filename() const {
    if (get_model_id() == nullptr) {
        return "";
    }

    const auto tmp_filename = get_model_id()->get_filename();

    if (tmp_filename.has_value()) {
        return tmp_filename->c_str();
    } else {
        return "";
    }
}

bool ModelWindow::has_unsaved_changes() const {
    const auto mdl = ui->block_graphics->get_model();
    if (mdl) {
        return mdl->get_unsaved_changes();
    } else {
        return false;
    }
}
