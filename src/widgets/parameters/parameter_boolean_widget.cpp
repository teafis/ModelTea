// SPDX-License-Identifier: GPL-3.0-only

#include "parameter_boolean_widget.h"
#include "ui_parameter_boolean_widget.h"

ParameterBooleanWidget::ParameterBooleanWidget(
    tmdl::Parameter* parameter,
    QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterBooleanWidget),
    parameter(parameter)
{
    ui->setupUi(this);

    if (parameter->get_data_type() != tmdl::ParameterValue::Type::BOOLEAN)
    {
        throw std::runtime_error("parameter must be a boolean type");
    }

    ui->lblName->setText(parameter->get_name().c_str());
    ui->chkSelection->setChecked(parameter->get_current_value().value.tf);

    connect(
        ui->chkSelection,
        &QCheckBox::stateChanged,
        this,
        &ParameterBooleanWidget::checkedStateChange);
}

void ParameterBooleanWidget::checkedStateChange(int state)
{
    if (state == Qt::CheckState::Checked)
    {
        parameter->set_value("1");
    }
    else if (state == Qt::CheckState::Unchecked)
    {
        parameter->set_value("0");
    }

    ui->chkSelection->setChecked(parameter->get_current_value().value.tf);

    emit parameterUpdated();
}

ParameterBooleanWidget::~ParameterBooleanWidget()
{
    delete ui;
}
