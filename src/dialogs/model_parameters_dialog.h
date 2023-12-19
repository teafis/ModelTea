// SPDX-License-Identifier: GPL-3.0-only

#ifndef MODEL_PARAMETERS_DIALOG_H
#define MODEL_PARAMETERS_DIALOG_H

#include <QDialog>

#include <tmdl/model.hpp>

namespace Ui {
class ModelParametersDialog;
}

class ModelParametersDialog : public QDialog {
    Q_OBJECT

public:
    explicit ModelParametersDialog(std::shared_ptr<tmdl::Model> model, const QString& model_name, QWidget* parent = nullptr);
    ~ModelParametersDialog();

    virtual void accept() override;

private:
    Ui::ModelParametersDialog* ui;
    std::shared_ptr<tmdl::Model> model;
};

#endif // MODEL_PARAMETERS_DIALOG_H
