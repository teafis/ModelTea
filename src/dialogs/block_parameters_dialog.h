// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCK_PARAMETER_DIALOG_H
#define BLOCK_PARAMETER_DIALOG_H

#include <QDialog>

#include "blocks/block_object.h"

namespace Ui {
class BlockParameterDialog;
}

class BlockParameterDialog : public QDialog {
    Q_OBJECT

public:
    explicit BlockParameterDialog(BlockObject* block, QWidget* parent = nullptr);

    ~BlockParameterDialog();

protected:
    void reloadParameters();

protected slots:
    void updateForParameters();

private:
    Ui::BlockParameterDialog* ui;
    BlockObject* block;
};

#endif // BLOCK_PARAMETER_DIALOG_H
