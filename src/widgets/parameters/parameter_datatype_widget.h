// SPDX-License-Identifier: GPL-3.0-only

#ifndef PARAMETER_DATATYPE_WIDGET_H
#define PARAMETER_DATATYPE_WIDGET_H

#include <QtWidgets/QWidget>

#include <parameter.hpp>

#include <unordered_map>

namespace Ui {
class ParameterDataTypeWidget;
}

class ParameterDataTypeWidget : public QWidget {
    Q_OBJECT

public:
    explicit ParameterDataTypeWidget(std::shared_ptr<tmdl::ParameterDataType> parameter, QWidget* parent = nullptr);
    ~ParameterDataTypeWidget();

protected slots:
    void selectionChanged(int);

signals:
    void parameterUpdated();

private:
    tmdl::DataType& get_value_data_type();

private:
    Ui::ParameterDataTypeWidget* ui;
    std::shared_ptr<tmdl::ParameterDataType> parameter;

    std::unordered_map<std::string, tmdl::DataType> map_values;
};

#endif // PARAMETER_DATATYPE_WIDGET_H
