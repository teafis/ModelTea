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

#include <iostream>
#include <QTextStream>
#include <QMessageBox>

void MainWindow::saveModel()
{
    QString saveName = QFileDialog::getSaveFileName(this, tr("Save Model"), QString(), "JSON (*.json); Any (*.*)");
    if (!saveName.isEmpty())
    {
        std::cout << "Saving to: \"" << saveName.toStdString() << "\"\n";
        const auto s = ui->block_graphics->getJsonString();
        std::cout << s << std::endl;

        QFile file(saveName);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, "error", file.errorString());
            return;
        }

        file.write(QString(s.c_str()).toUtf8());
        file.close();
    }
}

void MainWindow::openModel()
{
    QString openName = QFileDialog::getOpenFileName(this, tr("Open Model"), QString(), "JSON (*.json); Any (*.*)");
    if (!openName.isEmpty())
    {
        std::cout << "Loading from: \"" << openName.toStdString() << "\"\n";

        QFile file(openName);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, "error", file.errorString());
            return;
        }

        QTextStream stream(&file);
        QString data = stream.readAll();
        file.close();

        ui->block_graphics->fromJsonString(data.toStdString());
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
