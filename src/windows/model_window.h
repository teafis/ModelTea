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
    void updateMenuBars();

    void updateTitle();

public slots:
    void newModel();

    void saveModel();

    void saveModelAs();

    void openModel();

    void closeModel();

protected:
    void changeModel(std::shared_ptr<tmdl::Model> model);

public slots:
    void showErrors();

    void generateExecutor();

    void stepExecutor();

    void resetExecutor();

    void clearExecutor();

    void showLibrary();

    void showModelParameters();

    void showPlot();

    void addBlock(QString l, QString s);

signals:
    void executorEvent(SimEvent event);

    void modelChanged();

private:
    Ui::ModelWindow *ui;
    QString filename;

    ModelDiagnosticsDialog* window_diagnostics = nullptr;
    BlockSelectorDialog* window_library = nullptr;
    PlotWindow* window_plot = nullptr;

    std::shared_ptr<tmdl::ExecutionState> executor;

    bool changeFlag;
};

#endif // MODEL_WINDOW_H
