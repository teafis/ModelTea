#ifndef CONNECTION_PARAMETERS_DIALOG_H
#define CONNECTION_PARAMETERS_DIALOG_H

#include <QDialog>

#include "../blocks/connector_object.h"

namespace Ui {
class ConnectionParametersDialog;
}

class ConnectionParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionParametersDialog(ConnectorObject* conn, QWidget *parent = nullptr);
    ~ConnectionParametersDialog();

public slots:
    virtual void accept() override;

private:
    Ui::ConnectionParametersDialog *ui;
    ConnectorObject* const connector;
};

#endif // CONNECTION_PARAMETERS_DIALOG_H
