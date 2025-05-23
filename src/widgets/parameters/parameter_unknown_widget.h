// SPDX-License-Identifier: GPL-3.0-only

#ifndef PARAMETER_UNKNOWN_WIDGET_H
#define PARAMETER_UNKNOWN_WIDGET_H

#include <QWidget>

#include <parameter.hpp>

namespace Ui {
class ParameterUnknownWidget;
}

class ParameterUnknownWidget : public QWidget {
    Q_OBJECT

public:
    explicit ParameterUnknownWidget(std::shared_ptr<mtea::Parameter> parameter, QWidget* parent = nullptr);

    ~ParameterUnknownWidget();

private:
    Ui::ParameterUnknownWidget* ui;
};

#endif // PARAMETER_UNKNOWN_WIDGET_H
