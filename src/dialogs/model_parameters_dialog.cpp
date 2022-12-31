// SPDX-License-Identifier: GPL-3.0-only

#include "model_parameters_dialog.h"
#include "ui_model_parameters_dialog.h"

#include <tmdl/library_manager.hpp>

#include <QMessageBox>


ModelParametersDialog::ModelParametersDialog(std::shared_ptr<tmdl::Model> model, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ModelParametersDialog),
    model(model)
{
    ui->setupUi(this);

    ui->nameLineEdit->setText(QString(model->get_name().c_str()));
    ui->descriptionTextEdit->document()->setPlainText(QString(model->get_description().c_str()));

    setAttribute(Qt::WA_DeleteOnClose, true);
}

void ModelParametersDialog::accept()
{
    const auto& model_lib = tmdl::LibraryManager::get_instance().default_model_library();

    std::string new_name = ui->nameLineEdit->text().toStdString();

    if (model_lib->has_block(new_name))
    {
        QMessageBox::warning(this, "error", QString("Name %1 already exists in library").arg(new_name.c_str()));
        return;
    }

    model->set_name(ui->nameLineEdit->text().toStdString());
    model->set_description(ui->descriptionTextEdit->document()->toPlainText().toStdString());

    QDialog::accept();
}

ModelParametersDialog::~ModelParametersDialog()
{
    delete ui;
}
