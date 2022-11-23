// SPDX-License-Identifier: GPL-3.0-only

#include "main_window.h"
#include "./ui_main_window.h"

#include <QDebug>
#include <QIcon>
#include <QPixmap>

#include "main_window_icon.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Setup the main UI
    ui->setupUi(this);

    // Attempt to
    QPixmap pixmap;
    bool result = pixmap.loadFromData(
                main_window_icon::icon_png,
                main_window_icon::icon_png_len);
    if (result)
    {
        setWindowIcon(QIcon(pixmap));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
