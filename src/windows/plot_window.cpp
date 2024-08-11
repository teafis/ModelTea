// SPDX-License-Identifier: GPL-3.0-only

#include "plot_window.h"
#include "ui_plot_window.h"

#include <optional>

#include <value.hpp>

PlotWindow::PlotWindow(std::shared_ptr<tmdl::ExecutionState> execution, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::PlotWindow), execution_state(execution) {
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, true);

    auto* chart = new QChart();
    ui->chartView->setChart(chart);

    y_min = -1.0;
    y_max = 1.0;

    const auto names = execution_state->get_variable_names();

    list_model = new PlotVariableSelectionModel(this);

    for (const auto& n : names) {
        const auto name = QString(n.c_str());
        auto* s = new QLineSeries();
        s->setName(name);

        series.push_back(s);
        chart->addSeries(s);

        list_model->addItem(ItemSelector{.var = execution_state->get_variable_for_name(n), .name = name, .series = s});
    }

    chart->createDefaultAxes();

    ui->outputSelectionView->setModel(list_model);

    ui->outputSelectionView->selectionModel();

    connect(ui->outputSelectionView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PlotWindow::seriesListChanged);

    ui->outputSelectionView->selectAll();
}

PlotWindow::~PlotWindow() { delete ui; }

void PlotWindow::keyPressEvent(QKeyEvent* event) { QMainWindow::keyPressEvent(event); }

void PlotWindow::seriesListChanged() {
    const auto& items = list_model->items();
    for (const auto& it : items) {
        it.series->setVisible(false);
    }

    const auto& indexes = ui->outputSelectionView->selectionModel()->selectedIndexes();

    for (int i = 0; i < indexes.size(); ++i) {
        const auto it = qvariant_cast<ItemSelector>(list_model->data(indexes[i], Qt::UserRole));
        it.series->setVisible(true);
    }
}

static std::optional<double> double_from_variable(std::shared_ptr<const tmdl::ModelValue> ptr) {
    if (auto v = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::F64>>(ptr)) {
        return v->value;
    } else if (auto v = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::F32>>(ptr)) {
        return static_cast<double>(v->value);
    } else if (auto v = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::I32>>(ptr)) {
        return static_cast<double>(v->value);
    } else if (auto v = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::U32>>(ptr)) {
        return static_cast<double>(v->value);
    } else if (auto v = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::I16>>(ptr)) {
        return static_cast<double>(v->value);
    } else if (auto v = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::U16>>(ptr)) {
        return static_cast<double>(v->value);
    } else if (auto v = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::I8>>(ptr)) {
        return static_cast<double>(v->value);
    } else if (auto v = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::U8>>(ptr)) {
        return static_cast<double>(v->value);
    } else if (auto v = std::dynamic_pointer_cast<const tmdl::ModelValueBox<tmdl::DataType::BOOL>>(ptr)) {
        return (v->value) ? 1.0 : 0.0;
    } else {
        return std::nullopt;
    }
}

void PlotWindow::executorEvent(SimEvent event) {
    if (event.event() == SimEvent::EventType::Step) {
        const auto& names = execution_state->get_variable_names();

        for (size_t i = 0; i < names.size(); ++i) {
            const auto& n = names[i];
            auto var = execution_state->get_variable_for_name(n);

            const auto double_val = double_from_variable(var);

            const auto t = execution_state->get_current_time();

            series[i]->append(t, *double_val);
            ui->chartView->chart()->axes(Qt::Horizontal)[0]->setRange(0.0, t);

            y_min = std::min(*double_val, y_min);
            y_max = std::max(*double_val, y_max);

            ui->chartView->chart()->axes(Qt::Vertical)[0]->setRange(y_min, y_max);
        }
    } else if (event.event() == SimEvent::EventType::Reset) {
        for (int i = 0; i < series.size(); ++i) {
            series[i]->clear();
        }

        y_min = -1.0;
        y_max = 1.0;

        ui->chartView->chart()->axes(Qt::Horizontal)[0]->setRange(0.0, 1.0);
        ui->chartView->chart()->axes(Qt::Vertical)[0]->setRange(y_min, y_max);
    }
}
