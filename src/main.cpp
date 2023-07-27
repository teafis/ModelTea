// SPDX-License-Identifier: GPL-3.0-only

#include "windows/model_window.h"
#include "ui/app_icon.h"

#include "managers/window_manager.h"

#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap;
    if (pixmap.loadFromData(tsim_ui::icon_png, tsim_ui::icon_png_len))
    {
        a.setWindowIcon(QIcon(pixmap));
    }

    bool anyAdded = false;

    for (int i = 1; i < a.arguments().size(); ++i)
    {
        std::cout << a.arguments().at(i).toStdString() << std::endl;
        ModelWindow* w = new ModelWindow(WindowManager::instance().next_id());
        if (w->openModelFile(a.arguments().at(i)))
        {
            w->show();
            anyAdded = true;
        }
        else
        {
            w->close();
        }
    }

    if (!anyAdded)
    {
        ModelWindow* w = new ModelWindow(WindowManager::instance().next_id());
        w->show();
    }

    return a.exec();
}
