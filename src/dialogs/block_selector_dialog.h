// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCK_SELECTOR_DIALOG_H
#define BLOCK_SELECTOR_DIALOG_H

#include <QDialog>

#include <QListWidget>
#include <QListWidgetItem>

namespace Ui {
class BlockSelectorDialog;
}

class BlockSelectorDialog : public QDialog {
    Q_OBJECT

public:
    explicit BlockSelectorDialog(QWidget* parent = nullptr);
    ~BlockSelectorDialog();

protected slots:
    void librarySelectionUpdated();

    void itemSelected(QListWidgetItem* item);

public slots:
    void updateLibrary();

signals:
    void blockSelected(QString l, QString s);

private:
    Ui::BlockSelectorDialog* ui;
};

#endif // BLOCK_SELECTOR_DIALOG_H
