// SPDX-License-Identifier: GPL-3.0-only

#ifndef PARAMETER_BOOLEAN_WIDGET_H
#define PARAMETER_BOOLEAN_WIDGET_H

#include <QWidget>

#include <parameter.hpp>

namespace Ui {
class ParameterBooleanWidget;
}

class ParameterBooleanWidget : public QWidget {
    Q_OBJECT

public:
    explicit ParameterBooleanWidget(std::shared_ptr<tmdl::ParameterValue> parameter, QWidget* parent = nullptr);

    ~ParameterBooleanWidget();

protected slots:
    void checkedStateChange(int);

signals:
    void parameterUpdated();

private:
    bool& param_value();

private:
    Ui::ParameterBooleanWidget* ui;
    std::shared_ptr<tmdl::ParameterValue> parameter;
};

#endif // PARAMETER_BOOLEAN_WIDGET_H
