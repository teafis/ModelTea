// SPDX-License-Identifier: GPL-3.0-only

#include "model_error_dialog.h"
#include "ui_model_error_dialog.h"

ModelErrorDialog::ModelErrorDialog(
    const tmdl::Model* model,
    QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelErrorDialog),
    model(model)
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    ui->setupUi(this);
    modelUpdated();
}

void ModelErrorDialog::modelUpdated()
{
    ui->errors->clear();
    bool added = false;

    for (const auto& p : model->get_all_errors())
    {
        ui->errors->addItem(QString("Block %1: %2").arg(p->id).arg(p->message.c_str()));
        added = true;
    }

    if (!added)
    {
        ui->errors->addItem(QString("No Errors"));
    }
}

ModelErrorDialog::~ModelErrorDialog()
{
    delete ui;
}
