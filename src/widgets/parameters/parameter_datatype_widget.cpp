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

    if (parameter->get_value()->data_type() != tmdl::DataType::DATA_TYPE)
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
        if (kv.second == get_value_data_type())
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

ParameterDataTypeWidget::~ParameterDataTypeWidget()
{
    delete ui;
}

void ParameterDataTypeWidget::selectionChanged(int)
{
    const auto it = map_values.find(ui->comboBox->currentText().toStdString());
    tmdl::DataType new_dtype = tmdl::DataType::UNKNOWN;

    if (it != map_values.end())
    {
        new_dtype = it->second;
    }

    if (new_dtype != parameter->get_value()->data_type())
    {
        get_value_data_type() = new_dtype;
        emit parameterUpdated();
    }
}

tmdl::DataType& ParameterDataTypeWidget::get_value_data_type()
{
    return tmdl::ModelValue::get_inner_value<tmdl::DataType::DATA_TYPE>(parameter->get_value());
}
