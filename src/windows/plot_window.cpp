// SPDX-License-Identifier: GPL-3.0-only

#include "plot_window.h"
#include "ui_plot_window.h"

#include <optional>


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

static std::optional<double> double_from_variable(std::shared_ptr<const tmdl::ValueBox> ptr)
{
    if (auto v = std::dynamic_pointer_cast<const tmdl::ValueBoxType<double>>(ptr); v)
    {
        return v->value;
    }
    else if (auto v = std::dynamic_pointer_cast<const tmdl::ValueBoxType<float>>(ptr); v)
    {
        return static_cast<double>(v->value);
    }
    else if (auto v = std::dynamic_pointer_cast<const tmdl::ValueBoxType<int32_t>>(ptr); v)
    {
        return static_cast<double>(v->value);
    }
    else if (auto v = std::dynamic_pointer_cast<const tmdl::ValueBoxType<uint32_t>>(ptr); v)
    {
        return static_cast<double>(v->value);
    }
    else if (auto v = std::dynamic_pointer_cast<const tmdl::ValueBoxType<bool>>(ptr); v)
    {
        return (v->value) ? 1.0 : 0.0;
    }
    else
    {
        return std::nullopt;
    }
}

void PlotWindow::executorEvent(SimEvent event)
{
    if (event.event() == SimEvent::EventType::Step)
    {
        const auto vid = tmdl::VariableIdentifier
        {
            .block_id = /* TODO: model.get_id() */ 0,
            .output_port_num = 0
        };

        auto var = execution_state->variables->get_ptr(vid);

        const auto double_val = double_from_variable(var);

        const auto t = execution_state->state.get_time();

        series->append(t, *double_val);
        ui->chartView->chart()->axes(Qt::Horizontal)[0]->setRange(0.0, t);

        y_min = std::min(*double_val, y_min);
        y_max = std::max(*double_val, y_max);

        ui->chartView->chart()->axes(Qt::Vertical)[0]->setRange(y_min, y_max);
    }
    else if (event.event() == SimEvent::EventType::Reset)
    {
        series->clear();

        y_min = -1.0;
        y_max = 1.0;

        ui->chartView->chart()->axes(Qt::Horizontal)[0]->setRange(0.0, 1.0);
        ui->chartView->chart()->axes(Qt::Vertical)[0]->setRange(y_min, y_max);
    }
}

PlotWindow::~PlotWindow()
{
    delete ui;
}
