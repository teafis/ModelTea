// SPDX-License-Identifier: GPL-3.0-only

#ifndef PARAMETER_BOOLEAN_WIDGET_H
#define PARAMETER_BOOLEAN_WIDGET_H

#include <QWidget>

#include <tmdl/parameter.hpp>


namespace Ui {
class ParameterBooleanWidget;
}

class ParameterBooleanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterBooleanWidget(
        tmdl::Parameter* parameter,
        QWidget *parent = nullptr);

    ~ParameterBooleanWidget();

protected slots:
    void checkedStateChange(int);

signals:
    void parameterUpdated();

private:
    Ui::ParameterBooleanWidget *ui;
    tmdl::Parameter* parameter;
};

#endif // PARAMETER_BOOLEAN_WIDGET_H
