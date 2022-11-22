// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCK_LIBRARY_H
#define BLOCK_LIBRARY_H

#include <QDialog>

#include <tmdl/library.hpp>

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

    void set_library(const std::shared_ptr<const tmdl::LibraryBase> lib);

    ~BlockLibrary();

protected slots:
    void itemSelected(QListWidgetItem* item);

signals:
    void blockSelected(QString s);

private:
    Ui::BlockLibrary *ui;
    std::shared_ptr<const tmdl::LibraryBase> library;
};

#endif // BLOCK_LIBRARY_H
