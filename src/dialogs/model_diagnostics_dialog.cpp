// SPDX-License-Identifier: GPL-3.0-only

#include "model_diagnostics_dialog.h"
#include "ui_model_diagnostics_dialog.h"

#include "exceptions/model_exception.h"

ModelDiagnosticsDialog::ModelDiagnosticsDialog(const std::shared_ptr<const tmdl::Model> model, QWidget* parent)
    : QDialog(parent), ui(new Ui::ModelDiagnosticsDialog), model(model) {
    if (model == nullptr) {
        throw ModelException("model cannot be null");
    }

    ui->setupUi(this);
    updateDiagnostics();

    setAttribute(Qt::WA_DeleteOnClose, true);
}

void ModelDiagnosticsDialog::setModel(std::shared_ptr<const tmdl::Model> m) {
    if (m == nullptr) {
        throw ModelException("model cannot be null");
    } else {
        model = m;
        updateDiagnostics();
    }
}

void ModelDiagnosticsDialog::updateDiagnostics() {
    ui->errors->clear();
    bool added = false;

    for (const auto& p : model->get_all_errors()) {
        ui->errors->addItem(QString("Block %1: %2").arg(p->id).arg(p->message.c_str()));
        added = true;
    }

    if (!added) {
        ui->errors->addItem(QString("No Errors"));
    }
}

ModelDiagnosticsDialog::~ModelDiagnosticsDialog() { delete ui; }
