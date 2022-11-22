// SPDX-License-Identifier: GPL-3.0-only

#include "block_library.h"
#include "ui_block_library.h"

#include <QDebug>

BlockLibrary::BlockLibrary(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlockLibrary)
{
    ui->setupUi(this);

    connect(
        ui->listBlocks,
        &QListWidget::itemDoubleClicked,
        this,
        &BlockLibrary::itemSelected);
}

void BlockLibrary::set_library(const std::shared_ptr<const tmdl::LibraryBase> lib)
{
    library = lib;
    ui->listBlocks->clear();

    for (const auto& s : library->get_block_names())
    {
        ui->listBlocks->addItem(s.c_str());
    }
}

void BlockLibrary::itemSelected(QListWidgetItem* item)
{
    if (item != nullptr)
    {
        emit blockSelected(item->text());
    }
}

BlockLibrary::~BlockLibrary()
{
    delete ui;
}
