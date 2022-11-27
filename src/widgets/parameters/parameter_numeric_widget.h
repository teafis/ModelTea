// SPDX-License-Identifier: GPL-3.0-only

#ifndef PARAMETER_NUMERIC_WIDGET_H
#define PARAMETER_NUMERIC_WIDGET_H

#include <QWidget>

#include <tmdl/parameter.hpp>


namespace Ui {
class ParameterNumericWidget;
}

class ParameterNumericWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterNumericWidget(
        std::shared_ptr<tmdl::Parameter> parameter,
        QWidget *parent = nullptr);

    ~ParameterNumericWidget();

protected slots:
    void textChanged();

signals:
    void parameterUpdated();

private:
    Ui::ParameterNumericWidget *ui;
    std::shared_ptr<tmdl::Parameter> parameter;
};

#endif // PARAMETER_NUMERIC_WIDGET_H
