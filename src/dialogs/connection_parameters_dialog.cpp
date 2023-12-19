#include "connection_parameters_dialog.h"
#include "ui_connection_parameters_dialog.h"

#include <tmdl/values/identifiers.hpp>

#include <QMessageBox>

ConnectionParametersDialog::ConnectionParametersDialog(ConnectorBlockObject* conn, QWidget* parent)
    : QDialog(parent), ui(new Ui::ConnectionParametersDialog), connector(conn) {
    ui->setupUi(this);
    ui->lineEditConnectionName->setText(connector->get_name());

    setAttribute(Qt::WA_DeleteOnClose, true);
}

void ConnectionParametersDialog::accept() {
    const std::string n = ui->lineEditConnectionName->text().toStdString();
    try {
        connector->set_name(n.c_str());
        QDialog::accept();
    } catch (const tmdl::ModelException& err) {
        QMessageBox::warning(this, "error", err.what());
    }
}

ConnectionParametersDialog::~ConnectionParametersDialog() { delete ui; }
