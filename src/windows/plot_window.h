// SPDX-License-Identifier: GPL-3.0-only

#ifndef PLOT_WINDOW_H
#define PLOT_WINDOW_H

#include <QMainWindow>

#include <QVector>
#include <QtCharts>

#include <execution_state.hpp>

#include "events/sim_event.h"

#include "models/plot_variable_selection_model.h"

namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit PlotWindow(std::shared_ptr<mtea::ExecutionState> execution, QWidget* parent = nullptr);
    ~PlotWindow();

    virtual void keyPressEvent(QKeyEvent* event) override;

public slots:
    void executorEvent(SimEvent event);

protected slots:
    void seriesListChanged();

private:
    Ui::PlotWindow* ui;
    QVector<QLineSeries*> series;

    std::shared_ptr<mtea::ExecutionState> execution_state;

    PlotVariableSelectionModel* list_model;

    double y_min;
    double y_max;
};

#endif // PLOT_WINDOW_H
