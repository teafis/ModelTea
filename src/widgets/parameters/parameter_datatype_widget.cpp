// SPDX-License-Identifier: GPL-3.0-only

#include "parameter_datatype_widget.h"
#include "ui_parameter_datatype_widget.h"

#include "exceptions/block_object_exception.h"

ParameterDataTypeWidget::ParameterDataTypeWidget(std::shared_ptr<tmdl::ParameterDataType> parameter, QWidget* parent)
    : QWidget(parent), ui(new Ui::ParameterDataTypeWidget), parameter(parameter) {
    ui->setupUi(this);

    std::optional<int> selected_index{};

    for (const auto dt_meta : mt::stdlib::get_meta_types()) {
        if (map_values.contains(dt_meta->get_name())) {
            throw BlockObjectException(fmt::format("duplicate name provided for {}", dt_meta->get_name()));
        }

        if (parameter->get_type() == dt_meta->get_data_type()){
            selected_index = map_values.size();
        }

        ui->comboBox->addItem(dt_meta->get_name());
        map_values.insert({dt_meta->get_name(), dt_meta->get_data_type()});
    }

    ui->comboBox->addItem("none");

    if (selected_index.has_value()) {
        ui->comboBox->setCurrentIndex(*selected_index);
    } else {
        ui->comboBox->setCurrentIndex(map_values.size());
    }

    connect(ui->comboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ParameterDataTypeWidget::selectionChanged);
}

ParameterDataTypeWidget::~ParameterDataTypeWidget() { delete ui; }

void ParameterDataTypeWidget::selectionChanged(int) {
    const auto it = map_values.find(ui->comboBox->currentText().toStdString());
    tmdl::DataType new_dtype = tmdl::DataType::NONE;

    if (it != map_values.end()) {
        new_dtype = it->second;
    }

    if (new_dtype != parameter->get_type()) {
        get_value_data_type() = new_dtype;
        emit parameterUpdated();
    }
}

tmdl::DataType& ParameterDataTypeWidget::get_value_data_type() {
    return parameter->get_type();
}
