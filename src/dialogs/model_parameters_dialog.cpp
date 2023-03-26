// SPDX-License-Identifier: GPL-3.0-only

#include "model_parameters_dialog.h"
#include "ui_model_parameters_dialog.h"

#include <tmdl/library_manager.hpp>
#include <tmdl/model_exception.hpp>

#include <QMessageBox>


ModelParametersDialog::ModelParametersDialog(std::shared_ptr<tmdl::Model> model, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ModelParametersDialog),
    model(model)
{
    ui->setupUi(this);

    ui->nameLineEdit->setText(QString(model->get_name().c_str()));
    ui->descriptionTextEdit->document()->setPlainText(QString(model->get_description().c_str()));
    ui->dtLineEdit->setText(QString("%1").arg(model->get_preferred_dt()));

    setAttribute(Qt::WA_DeleteOnClose, true);
}

void ModelParametersDialog::accept()
{
    const auto& model_lib = tmdl::LibraryManager::get_instance().default_model_library();

    std::string new_name = ui->nameLineEdit->text().toStdString();

    if (model_lib->has_block(new_name) && new_name != model->get_name())
    {
        QMessageBox::warning(this, "error", QString("Name %1 already exists in library").arg(new_name.c_str()));
        return;
    }

    bool success = false;
    const double dtVal = ui->dtLineEdit->text().toDouble(&success);

    if (!success)
    {
        QMessageBox::warning(this, "error", QString("Unable to convert `%1` to double").arg(ui->dtLineEdit->text()));
        return;
    }

    const std::string orig_name = model->get_name();
    const std::string orig_desc = model->get_description();
    const double orig_dt = model->get_preferred_dt();

    try
    {
        model->set_name(ui->nameLineEdit->text().toStdString());
        model->set_description(ui->descriptionTextEdit->document()->toPlainText().toStdString());
        model->set_preferred_dt(dtVal);
    }
    catch (const tmdl::ModelException& ex)
    {
        model->set_name(orig_name);
        model->set_description(orig_desc);
        model->set_preferred_dt(orig_dt);

        QMessageBox::warning(this, "error", QString("%1").arg(ex.what()));
        return;
    }

    QDialog::accept();
}

ModelParametersDialog::~ModelParametersDialog()
{
    delete ui;
}
