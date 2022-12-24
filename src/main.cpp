// SPDX-License-Identifier: GPL-3.0-only

#include "windows/model_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ModelWindow w;
    w.show();
    return a.exec();
}
