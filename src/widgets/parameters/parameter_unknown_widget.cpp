// SPDX-License-Identifier: GPL-3.0-only

#include "parameter_unknown_widget.h"
#include "ui_parameter_unknown_widget.h"

ParameterUnknownWidget::ParameterUnknownWidget(std::shared_ptr<mtea::Parameter> parameter, QWidget* parent)
    : QWidget(parent), ui(new Ui::ParameterUnknownWidget) {
    ui->setupUi(this);

    ui->lblName->setText(parameter->get_name().c_str());
}

ParameterUnknownWidget::~ParameterUnknownWidget() { delete ui; }
