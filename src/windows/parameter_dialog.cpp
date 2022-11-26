// SPDX-License-Identifier: GPL-3.0-only

#include "parameter_dialog.h"
#include "ui_parameter_dialog.h"

#include "widgets/parameters/parameter_boolean_widget.h"
#include "widgets/parameters/parameter_numeric_widget.h"
#include "widgets/parameters/parameter_datatype_widget.h"
#include "widgets/parameters/parameter_unknown_widget.h"


ParameterDialog::ParameterDialog(
    BlockObject* block,
    QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterDialog),
    block(block)
{
    ui->setupUi(this);

    ui->lblBlockTitle->setText(block->get_block()->get_name().c_str());

    reloadParameters();

    setAttribute(Qt::WA_DeleteOnClose, true);
}

void ParameterDialog::reloadParameters()
{
    block->update_block();

    while (QLayoutItem* item = ui->parameterItemLayout->itemAt(0))
    {
        QWidget* w = item->widget();
        ui->parameterItemLayout->removeWidget(w);
        w->deleteLater();
    }

    for (auto prm : block->get_block()->get_parameters())
    {
        switch (prm->get_value().dtype)
        {
        case tmdl::ParameterValue::Type::BOOLEAN:
        {
            ParameterBooleanWidget* w = new ParameterBooleanWidget(prm, this);
            ui->parameterItemLayout->addWidget(w);
            connect(
                w,
                &ParameterBooleanWidget::parameterUpdated,
                this,
                &ParameterDialog::updateForParameters);
            break;
        }
        case tmdl::ParameterValue::Type::SINGLE:
        case tmdl::ParameterValue::Type::DOUBLE:
        case tmdl::ParameterValue::Type::INT32:
        case tmdl::ParameterValue::Type::UINT32:
        {
            ParameterNumericWidget* w = new ParameterNumericWidget(prm, this);
            ui->parameterItemLayout->addWidget(w);
            connect(
                w,
                &ParameterNumericWidget::parameterUpdated,
                this,
                &ParameterDialog::updateForParameters);
            break;
        }
        case tmdl::ParameterValue::Type::DATA_TYPE:
        {
            ParameterDataTypeWidget* w = new ParameterDataTypeWidget(prm, this);
            ui->parameterItemLayout->addWidget(w);
            connect(
                w,
                &ParameterDataTypeWidget::parameterUpdated,
                this,
                &ParameterDialog::updateForParameters);
            break;
        }
        default:
            ParameterUnknownWidget* w = new ParameterUnknownWidget(prm, this);
            ui->parameterItemLayout->addWidget(w);
            break;
        }
    }
}

void ParameterDialog::updateForParameters()
{
    reloadParameters();
    block->update();
}

ParameterDialog::~ParameterDialog()
{
    delete ui;
}
