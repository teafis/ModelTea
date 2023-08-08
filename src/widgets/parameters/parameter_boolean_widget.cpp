// SPDX-License-Identifier: GPL-3.0-only

#include "parameter_boolean_widget.h"
#include "ui_parameter_boolean_widget.h"

#include "exceptions/block_object_exception.h"

ParameterBooleanWidget::ParameterBooleanWidget(
    std::shared_ptr<tmdl::Parameter> parameter,
    QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterBooleanWidget),
    parameter(parameter)
{
    ui->setupUi(this);

    if (parameter->get_value().dtype != tmdl::ParameterValue::Type::BOOLEAN)
    {
        throw BlockObjectException("parameter must be a boolean type");
    }

    ui->lblName->setText(parameter->get_name().c_str());
    ui->chkSelection->setChecked(parameter->get_value().value.tf);

    connect(
        ui->chkSelection,
        &QCheckBox::stateChanged,
        this,
        &ParameterBooleanWidget::checkedStateChange);
}

void ParameterBooleanWidget::checkedStateChange(int state)
{
    parameter->get_value().value.tf = state == Qt::CheckState::Checked;
    ui->chkSelection->setChecked(parameter->get_value().value.tf);

    emit parameterUpdated();
}

ParameterBooleanWidget::~ParameterBooleanWidget()
{
    delete ui;
}
