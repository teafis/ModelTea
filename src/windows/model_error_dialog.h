// SPDX-License-Identifier: GPL-3.0-only

#ifndef MODEL_ERROR_DIALOG_H
#define MODEL_ERROR_DIALOG_H

#include <QDialog>

#include <tmdl/model.hpp>


namespace Ui {
class ModelErrorDialog;
}

class ModelErrorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelErrorDialog(
        const tmdl::Model* model,
        QWidget *parent = nullptr);
    ~ModelErrorDialog();

public slots:
    void modelUpdated();

private:
    Ui::ModelErrorDialog *ui;
    const tmdl::Model* model;
};

#endif // MODEL_ERROR_DIALOG_H
