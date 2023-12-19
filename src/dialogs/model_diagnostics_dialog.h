// SPDX-License-Identifier: GPL-3.0-only

#ifndef MODEL_DIAGNOSTIC_DIALOG_H
#define MODEL_DIAGNOSTIC_DIALOG_H

#include <QDialog>

#include <memory>

#include <tmdl/model.hpp>

namespace Ui {
class ModelDiagnosticsDialog;
}

class ModelDiagnosticsDialog : public QDialog {
    Q_OBJECT

public:
    explicit ModelDiagnosticsDialog(const std::shared_ptr<const tmdl::Model> model, QWidget* parent = nullptr);
    ~ModelDiagnosticsDialog();

public:
    void setModel(std::shared_ptr<const tmdl::Model> m);

public slots:
    void updateDiagnostics();

private:
    Ui::ModelDiagnosticsDialog* ui;
    std::shared_ptr<const tmdl::Model> model;
};

#endif // MODEL_DIAGNOSTIC_DIALOG_H
