// SPDX-License-Identifier: GPL-3.0-only

#ifndef PLOT_WINDOW_H
#define PLOT_WINDOW_H

#include <QMainWindow>

#include <QtCharts>

#include <tmdl/execution_state.hpp>

#include "events/sim_event.h"


namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(
        std::shared_ptr<tmdl::ExecutionState> execution,
        QWidget *parent = nullptr);
    ~PlotWindow();

public slots:
    void executorEvent(SimEvent event);

    //void resetPlot();

private:
    Ui::PlotWindow* ui;
    QLineSeries* series;

    std::shared_ptr<tmdl::ExecutionState> execution_state;

    double y_min;
    double y_max;
};

#endif // PLOT_WINDOW_H
