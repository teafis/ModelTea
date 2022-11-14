// SPDX-License-Identifier: GPL-3.0-only

#include "windows/main_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}