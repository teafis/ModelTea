#include "connection_parameters_dialog.h"
#include "ui_connection_parameters_dialog.h"

#include <tmdl/util/identifiers.hpp>

#include <QMessageBox>


ConnectionParametersDialog::ConnectionParametersDialog(ConnectorObject* conn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionParametersDialog),
    connector(conn)
{
    ui->setupUi(this);
    ui->lineEditConnectionName->setText(connector->get_name());

    setAttribute(Qt::WA_DeleteOnClose, true);
}

void ConnectionParametersDialog::accept()
{
    const std::string n = ui->lineEditConnectionName->text().toStdString();
    if (!tmdl::Connection::is_valid_name(n))
    {
        QMessageBox::warning(this, "error", QString("Identifier `%1` is not a valid identifier").arg(n.c_str()));
    }
    else
    {
        connector->set_name(n.c_str());
        QDialog::accept();
    }
}

ConnectionParametersDialog::~ConnectionParametersDialog()
{
    delete ui;
}
