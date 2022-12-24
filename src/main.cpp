// SPDX-License-Identifier: GPL-3.0-only

#include "windows/model_window.h"
#include "ui/window_icon.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap;
    if (pixmap.loadFromData(tsim_ui::icon_png, tsim_ui::icon_png_len))
    {
        a.setWindowIcon(QIcon(pixmap));
    }

    ModelWindow w;
    w.show();
    return a.exec();
}
