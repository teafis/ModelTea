// SPDX-License-Identifier: GPL-3.0-only

#include "windows/model_window.h"

#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QIcon icon;

    const std::string icon_file = ":/icons/icon.png";
    if (QFile::exists(icon_file.c_str()))
    {
        icon = QIcon(icon_file.c_str());
    }
    else
    {
        std::cerr << "Unable to load icon file :-(\n";
        return 1;
    }

    a.setWindowIcon(icon);

    bool anyAdded = false;

    for (int i = 1; i < a.arguments().size(); ++i)
    {
        ModelWindow* w = new ModelWindow();
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
        ModelWindow* w = new ModelWindow();
        w->show();
    }

    return a.exec();
}
