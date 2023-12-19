// SPDX-License-Identifier: GPL-3.0-only

#include "block_selector_dialog.h"
#include "ui_block_selector_dialog.h"

#include <tmdl/library_manager.hpp>

BlockSelectorDialog::BlockSelectorDialog(QWidget* parent) : QDialog(parent), ui(new Ui::BlockSelectorDialog) {
    ui->setupUi(this);

    connect(ui->listBlocks, &QListWidget::itemDoubleClicked, this, &BlockSelectorDialog::itemSelected);

    setAttribute(Qt::WA_DeleteOnClose, true);

    updateLibrary();
}

BlockSelectorDialog::~BlockSelectorDialog() { delete ui; }

void BlockSelectorDialog::librarySelectionUpdated() {
    ui->listBlocks->clear();

    const tmdl::LibraryManager& manager = tmdl::LibraryManager::get_instance();

    const auto& items = ui->listLibraries->selectedItems();
    for (const auto* itm : std::as_const(items)) {
        const auto libName = itm->data(Qt::UserRole).toString().toStdString();
        const auto lib = manager.get_library(libName);

        for (const auto& n : lib->get_block_names()) {
            QListWidgetItem* item = new QListWidgetItem(QString("%1::%2").arg(libName.c_str()).arg(n.c_str()), ui->listBlocks);
            item->setData(Qt::UserRole + 0, QString(libName.c_str()));
            item->setData(Qt::UserRole + 1, QString(n.c_str()));
        }
        (void)itm;
    }
}

void BlockSelectorDialog::itemSelected(QListWidgetItem* item) {
    if (item != nullptr) {
        const QString libName = item->data(Qt::UserRole + 0).toString();
        const QString blockName = item->data(Qt::UserRole + 1).toString();

        emit blockSelected(libName, blockName);
    }
}

void BlockSelectorDialog::updateLibrary() {
    const tmdl::LibraryManager& manager = tmdl::LibraryManager::get_instance();
    ui->listLibraries->clear();
    for (const auto& n : manager.get_library_names()) {
        QListWidgetItem* item = new QListWidgetItem(QString(n.c_str()), ui->listLibraries);
        item->setData(Qt::UserRole, QString(n.c_str()));
    }

    connect(ui->listLibraries, &QListWidget::itemSelectionChanged, this, &BlockSelectorDialog::librarySelectionUpdated);

    ui->listLibraries->selectAll();
}
