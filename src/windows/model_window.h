// SPDX-License-Identifier: GPL-3.0-only

#ifndef MODEL_WINDOW_H
#define MODEL_WINDOW_H

#include <QMainWindow>

#include "dialogs/block_selector_dialog.h"
#include "dialogs/model_diagnostics_dialog.h"

#include "windows/plot_window.h"

#include <tmdl/block_interface.hpp>


QT_BEGIN_NAMESPACE
namespace Ui { class ModelWindow; }
QT_END_NAMESPACE

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

public slots:
    void saveModel();

    void saveModelAs();

    void openModel();

public slots:
    void showErrors();

    void generateExecutor();

    void stepExecutor();

    void clearExecutor();

    void showLibrary();

    void showModelParameters();

    void showPlot();

    void addBlock(QString l, QString s);

signals:
    void plotPointUpdated(const double t, const double y);

private:
    Ui::ModelWindow *ui;
    QString filename;

    ModelDiagnosticsDialog* window_errors = nullptr;
    BlockSelectorDialog* window_library = nullptr;
    PlotWindow* window_plot = nullptr;

    struct ExecutionState
    {
        std::shared_ptr<tmdl::VariableManager> variables;
        std::shared_ptr<tmdl::BlockExecutionInterface> model;
        tmdl::SimState state;
        uint64_t iterations;
    };

    std::unique_ptr<ExecutionState> executor;
};
#endif // MODEL_WINDOW_H
