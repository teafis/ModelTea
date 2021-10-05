#include "main_window.h"
#include "./ui_main_window.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::button_press_listener()
{
    qDebug() << "Button Pressed!";
}

MainWindow::~MainWindow()
{
    delete ui;
}

