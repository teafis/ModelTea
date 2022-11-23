#include "plot_window.h"
#include "ui_plot_window.h"

#include <QChart>


PlotWindow::PlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow)
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
}

void PlotWindow::addPlotPoint(const double t, const double y)
{
    series->append(t, y);
    ui->chartView->chart()->axes(Qt::Horizontal)[0]->setRange(0.0, t);
}

PlotWindow::~PlotWindow()
{
    delete ui;
}
