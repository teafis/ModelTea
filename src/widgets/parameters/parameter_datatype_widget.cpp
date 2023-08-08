// SPDX-License-Identifier: GPL-3.0-only

#include "parameter_datatype_widget.h"
#include "ui_parameter_datatype_widget.h"

#include "exceptions/block_object_exception.h"

ParameterDataTypeWidget::ParameterDataTypeWidget(
    std::shared_ptr<tmdl::Parameter> parameter,
    QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ParameterDataTypeWidget),
    parameter(parameter)
{
    ui->setupUi(this);

    if (parameter->get_value().dtype != tmdl::ParameterValue::Type::DATA_TYPE)
    {
        throw BlockObjectException("parameter must be a data type");
    }

    map_values =
    {
        {"Double", tmdl::DataType::DOUBLE},
        {"Single", tmdl::DataType::SINGLE},
        {"Int 32", tmdl::DataType::INT32},
        {"UInt 32", tmdl::DataType::UINT32},
        {"Boolean", tmdl::DataType::BOOLEAN},
        {"Unknown", tmdl::DataType::UNKNOWN}
    };

    for (const auto& kv : map_values)
    {
        ui->comboBox->addItem(kv.first.c_str());
    }

    std::string current_string;
    for (const auto& kv : map_values)
    {
        current_string = kv.first;
        if (kv.second == parameter->get_value().value.dtype)
        {
            break;
        }
    }

    ui->comboBox->setCurrentText(current_string.c_str());

    connect(
        ui->comboBox,
        qOverload<int>(&QComboBox::currentIndexChanged),
        this,
        &ParameterDataTypeWidget::selectionChanged);
}

void ParameterDataTypeWidget::selectionChanged(int)
{
    const auto it = map_values.find(ui->comboBox->currentText().toStdString());
    tmdl::DataType new_dtype = tmdl::DataType::UNKNOWN;

    if (it != map_values.end())
    {
        new_dtype = it->second;
    }

    if (new_dtype != parameter->get_value().value.dtype)
    {
        parameter->get_value().value.dtype = new_dtype;
        emit parameterUpdated();
    }
}

ParameterDataTypeWidget::~ParameterDataTypeWidget()
{
    delete ui;
}
