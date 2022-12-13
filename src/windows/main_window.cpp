// SPDX-License-Identifier: GPL-3.0-only

#include "main_window.h"
#include "./ui_main_window.h"

#include <QIcon>
#include <QPixmap>
#include <QFileDialog>

#include "main_window_icon.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Setup the main UI
    ui->setupUi(this);

    // Attempt to load the window icon
    QPixmap pixmap;
    bool result = pixmap.loadFromData(
        main_window_icon::icon_png,
        main_window_icon::icon_png_len);
    if (result)
    {
        setWindowIcon(QIcon(pixmap));
    }

    // Connect signals
    connect(
        ui->block_graphics,
        &BlockGraphicsView::generatedModelCreated,
        [this]() { updateMenuBars(true); });
    connect(
        ui->block_graphics,
        &BlockGraphicsView::generatedModelDestroyed,
        [this]() { updateMenuBars(false); });
}

void MainWindow::updateMenuBars(bool generatedAvailable)
{
    ui->menuModel->setEnabled(!generatedAvailable);
    ui->menuBlocks->setEnabled(!generatedAvailable);
    ui->menuSim->setEnabled(generatedAvailable);
}

#include <QDebug>

void MainWindow::saveModel()
{
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setNameFilter("JSON (*.json)");

    if (dialog->exec())
    {
        const auto s = ui->block_graphics->getJsonString();
        dialog->getSaveFileName();

        qDebug() << s.c_str();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    (void) event;
    ui->block_graphics->onClose();
}
