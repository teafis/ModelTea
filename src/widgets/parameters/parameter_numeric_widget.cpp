// SPDX-License-Identifier: GPL-3.0-only

#include "parameter_numeric_widget.h"
#include "ui_parameter_numeric_widget.h"

#include <model_exception.hpp>

#include <QMessageBox>

ParameterNumericWidget::ParameterNumericWidget(std::shared_ptr<mtea::ParameterValue> parameter, QWidget* parent)
    : QWidget(parent), ui(new Ui::ParameterNumericWidget), parameter(parameter) {
    ui->setupUi(this);

    ui->lblName->setText(parameter->get_name().c_str());
    ui->textEntry->setText(parameter->get_value()->to_string().c_str());

    connect(ui->textEntry, &QLineEdit::editingFinished, this, &ParameterNumericWidget::textChanged);
}

void ParameterNumericWidget::textChanged() {
    try {
        auto value = mtea::ModelValue::from_string(ui->textEntry->text().toStdString(), parameter->get_value()->data_type());

        parameter->set_value(std::move(value));
    } catch (const mtea::ModelException& ex) {
        auto* msg = new QMessageBox(this);
        msg->setText(ex.what());
        msg->setWindowTitle("Parameter Error");
        connect(msg, &QMessageBox::finished, [msg]() { msg->deleteLater(); });
        msg->open();
        return;
    }

    ui->textEntry->text() = parameter->get_value()->to_string().c_str();
    emit parameterUpdated();
}

ParameterNumericWidget::~ParameterNumericWidget() { delete ui; }
