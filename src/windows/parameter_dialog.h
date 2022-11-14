// SPDX-License-Identifier: GPL-3.0-only

#ifndef PARAMETER_DIALOG_H
#define PARAMETER_DIALOG_H

#include <QDialog>

#include "blocks/block_object.h"

namespace Ui {
class ParameterDialog;
}

class ParameterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterDialog(BlockObject* block, QWidget *parent = nullptr);
    ~ParameterDialog();

private:
    Ui::ParameterDialog *ui;
    BlockObject* block;
};

#endif // PARAMETER_DIALOG_H
