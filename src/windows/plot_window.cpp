// SPDX-License-Identifier: GPL-3.0-only

#include "plot_window.h"
#include "ui_plot_window.h"


PlotWindow::PlotWindow(
    std::shared_ptr<tmdl::ExecutionState> execution,
    QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow),
    execution_state(execution)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, true);

    auto* chart = new QChart();

    series = new QLineSeries();
    chart->addSeries(series);

    chart->createDefaultAxes();
    chart->setTitle("Testing Plot");

    chart->legend()->hide();

    ui->chartView->setChart(chart);

    y_min = -1.0;
    y_max = 1.0;
}

void PlotWindow::addPlotPoint(const double t, const double y)
{
    series->append(t, y);
    ui->chartView->chart()->axes(Qt::Horizontal)[0]->setRange(0.0, t);

    y_min = std::min(y, y_min);
    y_max = std::max(y, y_max);

    ui->chartView->chart()->axes(Qt::Vertical)[0]->setRange(y_min, y_max);
}

void PlotWindow::resetPlot()
{
    series->clear();

    y_min = -1.0;
    y_max = 1.0;

    ui->chartView->chart()->axes(Qt::Horizontal)[0]->setRange(0.0, 1.0);
    ui->chartView->chart()->axes(Qt::Vertical)[0]->setRange(y_min, y_max);
}

PlotWindow::~PlotWindow()
{
    delete ui;
}
