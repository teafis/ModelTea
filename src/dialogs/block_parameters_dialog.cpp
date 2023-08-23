// SPDX-License-Identifier: GPL-3.0-only

#include "block_parameters_dialog.h"
#include "ui_block_parameters_dialog.h"

#include "widgets/parameters/parameter_boolean_widget.h"
#include "widgets/parameters/parameter_numeric_widget.h"
#include "widgets/parameters/parameter_datatype_widget.h"
#include "widgets/parameters/parameter_unknown_widget.h"


BlockParameterDialog::BlockParameterDialog(
    BlockObject* block,
    QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlockParameterDialog),
    block(block)
{
    ui->setupUi(this);

    ui->lblBlockTitle->setText(block->get_block()->get_name().c_str());

    reloadParameters();

    setAttribute(Qt::WA_DeleteOnClose, true);
}

void BlockParameterDialog::reloadParameters()
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
        if (!prm->get_enabled())
        {
            continue;
        }

        switch (prm->get_value()->data_type())
        {
        case tmdl::DataType::BOOLEAN:
        {
            ParameterBooleanWidget* w = new ParameterBooleanWidget(prm, this);
            ui->parameterItemLayout->addWidget(w);
            connect(
                w,
                &ParameterBooleanWidget::parameterUpdated,
                this,
                &BlockParameterDialog::updateForParameters);
            break;
        }
        case tmdl::DataType::SINGLE:
        case tmdl::DataType::DOUBLE:
        case tmdl::DataType::INT32:
        case tmdl::DataType::UINT32:
        {
            ParameterNumericWidget* w = new ParameterNumericWidget(prm, this);
            ui->parameterItemLayout->addWidget(w);
            connect(
                w,
                &ParameterNumericWidget::parameterUpdated,
                this,
                &BlockParameterDialog::updateForParameters);
            break;
        }
        case tmdl::DataType::DATA_TYPE:
        {
            ParameterDataTypeWidget* w = new ParameterDataTypeWidget(prm, this);
            ui->parameterItemLayout->addWidget(w);
            connect(
                w,
                &ParameterDataTypeWidget::parameterUpdated,
                this,
                &BlockParameterDialog::updateForParameters);
            break;
        }
        default:
            ParameterUnknownWidget* w = new ParameterUnknownWidget(prm, this);
            ui->parameterItemLayout->addWidget(w);
            break;
        }
    }
}

void BlockParameterDialog::updateForParameters()
{
    reloadParameters();
    block->update();
}

BlockParameterDialog::~BlockParameterDialog()
{
    delete ui;
}
