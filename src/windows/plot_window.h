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

private:
    Ui::PlotWindow *ui;
    QLineSeries* series;
};

#endif // PLOT_WINDOW_H
