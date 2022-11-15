// SPDX-License-Identifier: GPL-3.0-only

#include "parameter_numeric_widget.h"
#include "ui_parameter_numeric_widget.h"

ParameterNumericWidget::ParameterNumericWidget(
    tmdl::Parameter* parameter,
    QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterNumericWidget),
    parameter(parameter)
{
    ui->setupUi(this);

    if (parameter->get_data_type() != tmdl::ParameterValue::Type::DOUBLE)
    {
        throw std::runtime_error("parameter must be a boolean type");
    }

    ui->lblName->setText(parameter->get_name().c_str());
    ui->textEntry->setText(parameter->get_current_value_string().c_str());

    connect(
        ui->textEntry,
        &QLineEdit::editingFinished,
        this,
        &ParameterNumericWidget::textChanged);
}

void ParameterNumericWidget::textChanged()
{
    parameter->set_value(ui->textEntry->text().toStdString());
    emit parameterUpdated();
}

ParameterNumericWidget::~ParameterNumericWidget()
{
    delete ui;
}
