// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCK_LIBRARY_H
#define BLOCK_LIBRARY_H

#include <QDialog>

#include <QListWidget>
#include <QListWidgetItem>

namespace Ui {
class BlockLibrary;
}

class BlockLibrary : public QDialog
{
    Q_OBJECT

public:
    explicit BlockLibrary(QWidget *parent = nullptr);
    ~BlockLibrary();

protected slots:
    void librarySelectionUpdated();

    void itemSelected(QListWidgetItem* item);

signals:
    void blockSelected(QString l, QString s);

private:
    Ui::BlockLibrary *ui;
};

#endif // BLOCK_LIBRARY_H
