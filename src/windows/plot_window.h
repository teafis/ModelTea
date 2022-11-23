#ifndef PLOT_WINDOW_H
#define PLOT_WINDOW_H

#include <QMainWindow>

#include <QtCharts>


namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = nullptr);
    ~PlotWindow();

public slots:
    void addPlotPoint(const double t, const double y);

    void resetPlot();

private:
    Ui::PlotWindow *ui;
    QLineSeries* series;
    double y_min;
    double y_max;
};

#endif // PLOT_WINDOW_H
