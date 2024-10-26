// SPDX-License-Identifier: GPL-3.0-only

#include "block_parameters_dialog.h"
#include "ui_block_parameters_dialog.h"

#include "widgets/parameters/parameter_boolean_widget.h"
#include "widgets/parameters/parameter_datatype_widget.h"
#include "widgets/parameters/parameter_numeric_widget.h"
#include "widgets/parameters/parameter_unknown_widget.h"

BlockParameterDialog::BlockParameterDialog(BlockObject* block, QWidget* parent)
    : QDialog(parent), ui(new Ui::BlockParameterDialog), block(block) {
    ui->setupUi(this);

    ui->lblBlockTitle->setText(block->get_block()->get_name().c_str());

    reloadParameters();
}

void BlockParameterDialog::reloadParameters() {
    block->updateBlock();

    while (QLayoutItem* item = ui->parameterItemLayout->itemAt(0)) {
        QWidget* w = item->widget();
        ui->parameterItemLayout->removeWidget(w);
        w->deleteLater();
    }

    for (auto prm : block->get_block()->get_parameters()) {
        if (!prm->get_enabled()) {
            continue;
        }

        QWidget* widget = nullptr;

        if (const auto prm_mdl = std::dynamic_pointer_cast<mtea::ParameterValue>(prm)) {
            const auto dt = prm_mdl->get_value()->data_type();
            const auto dt_meta = mtea::get_meta_type(dt);

            if (dt == mtea::DataType::BOOL) {
                ParameterBooleanWidget* w = new ParameterBooleanWidget(prm_mdl, this);
                connect(w, &ParameterBooleanWidget::parameterUpdated, this, &BlockParameterDialog::updateForParameters);
                widget = w;
            } else if (dt_meta && dt_meta->get_is_numeric()) {
                ParameterNumericWidget* w = new ParameterNumericWidget(prm_mdl, this);
                connect(w, &ParameterNumericWidget::parameterUpdated, this, &BlockParameterDialog::updateForParameters);
                widget = w;
            }
        } else if (const auto prm_dt = std::dynamic_pointer_cast<mtea::ParameterDataType>(prm)) {
            ParameterDataTypeWidget* w = new ParameterDataTypeWidget(prm_dt, this);
            connect(w, &ParameterDataTypeWidget::parameterUpdated, this, &BlockParameterDialog::updateForParameters);
            widget = w;
        }

        if (widget == nullptr) {
            widget = new ParameterUnknownWidget(prm, this);
        }

        ui->parameterItemLayout->addWidget(widget);
    }
}

void BlockParameterDialog::updateForParameters() {
    reloadParameters();
    block->update();
}

BlockParameterDialog::~BlockParameterDialog() { delete ui; }
