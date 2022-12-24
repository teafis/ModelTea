#include "model_parameters_dialog.h"
#include "ui_model_parameters_dialog.h"

ModelParametersDialog::ModelParametersDialog(std::shared_ptr<tmdl::Model> model, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ModelParametersDialog),
    model(model)
{
    ui->setupUi(this);

    ui->nameLineEdit->setText(QString(model->get_name().c_str()));
    ui->descriptionTextEdit->document()->setPlainText(QString(model->get_description().c_str()));

    connect(
        this,
        &ModelParametersDialog::accepted,
        this,
        &ModelParametersDialog::onAccept);

    setAttribute(Qt::WA_DeleteOnClose, true);
}

void ModelParametersDialog::onAccept()
{
    model->set_name(ui->nameLineEdit->text().toStdString());
    model->set_description(ui->descriptionTextEdit->document()->toPlainText().toStdString());
}

ModelParametersDialog::~ModelParametersDialog()
{
    delete ui;
}
