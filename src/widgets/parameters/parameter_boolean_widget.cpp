// SPDX-License-Identifier: GPL-3.0-only

#include "parameter_boolean_widget.h"
#include "ui_parameter_boolean_widget.h"

#include "exceptions/block_object_exception.h"

ParameterBooleanWidget::ParameterBooleanWidget(std::shared_ptr<mtea::ParameterValue> parameter, QWidget* parent)
    : QWidget(parent), ui(new Ui::ParameterBooleanWidget), parameter(parameter) {
    ui->setupUi(this);

    if (parameter->get_value()->data_type() != mtea::DataType::BOOL) {
        throw BlockObjectException("parameter must be a boolean type");
    }

    ui->lblName->setText(parameter->get_name().c_str());
    ui->chkSelection->setChecked(param_value());

    connect(ui->chkSelection, &QCheckBox::stateChanged, this, &ParameterBooleanWidget::checkedStateChange);
}

ParameterBooleanWidget::~ParameterBooleanWidget() { delete ui; }

void ParameterBooleanWidget::checkedStateChange(int state) {
    param_value() = state == Qt::CheckState::Checked;
    ui->chkSelection->setChecked(param_value());

    emit parameterUpdated();
}

bool& ParameterBooleanWidget::param_value() { return mtea::ModelValue::get_inner_value<mtea::DataType::BOOL>(parameter->get_value()); }
