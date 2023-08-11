// SPDX-License-Identifier: GPL-3.0-only

#ifndef MODEL_WINDOW_H
#define MODEL_WINDOW_H

#include <QMainWindow>

#include "events/sim_event.h"

#include "dialogs/block_selector_dialog.h"
#include "dialogs/model_diagnostics_dialog.h"

#include "windows/plot_window.h"

#include <tmdl/block_interface.hpp>
#include <tmdl/execution_state.hpp>


namespace Ui { class ModelWindow; }

class ModelWindow : public QMainWindow
{
    Q_OBJECT

public:
    ModelWindow(QWidget *parent = nullptr);
    ~ModelWindow();

    void closeEvent(QCloseEvent* event) override;

    void keyPressEvent(QKeyEvent *event) override;

protected slots:
    void updateWindowItems();

    void setChangedFlag();

public slots:
    void newModel();

    void saveModel();

    void saveModelAs();

    void openFileDialog();

    bool openModelFile(QString openFilename);

    bool openModel(std::shared_ptr<tmdl::Model> model);

    void closeModel();

    void saveCode();

protected:
    void changeModel(std::shared_ptr<tmdl::Model> model);

public slots:
    void showDiagnostics();

    void generateExecutor();

    void stepExecutor();

    void resetExecutor();

    void clearExecutor();

    void showLibrary();

    void showModelParameters();

    void showPlot();

    void addBlock(QString l, QString s);

private slots:
    void executorFlagSet();

public:
    QString currentModelName() const;

signals:
    void executorEvent(SimEvent event);

    void modelChanged();

private:
    tmdl::Model* get_model_id();
    const tmdl::Model* get_model_id() const;
    QString get_filename() const;

private:
    Ui::ModelWindow *ui;

    ModelDiagnosticsDialog* window_diagnostics = nullptr;
    BlockSelectorDialog* window_library = nullptr;
    PlotWindow* window_plot = nullptr;

    std::shared_ptr<tmdl::ExecutionState> executor;

    bool changeFlag;

    static const std::string default_extension;
    static const QString default_file_filter;
};

#endif // MODEL_WINDOW_H
