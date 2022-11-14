// SPDX-License-Identifier: GPL-3.0-only

#include "parameter_dialog.h"
#include "ui_parameter_dialog.h"

ParameterDialog::ParameterDialog(BlockObject* block, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterDialog),
    block(block)
{
    ui->setupUi(this);

    ui->lblBlockTitle->setText(block->get_block()->get_name().c_str());
    ui->lblBlockDescription->setText("Test Description\nNewline Here!");
}

ParameterDialog::~ParameterDialog()
{
    delete ui;
}
