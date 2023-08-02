// SPDX-License-Identifier: GPL-3.0-only

#include "windows/model_window.h"

#include "managers/window_manager.h"

#include <QApplication>

#include <array>
#include <iostream>
#include <optional>

#include <fmt/format.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    std::array<std::string, 2> check_files = {
        "macos",
        "icon",
    };
    std::optional<QIcon> icon;

    for (const auto& n : check_files)
    {
        const std::string f = fmt::format(":/icons/{}.png", n);
        if (QFile::exists(f.c_str()))
        {
            icon.emplace(f.c_str());
            break;
        }
    }

    if (icon)
    {
        a.setWindowIcon(*icon);
    }
    else
    {
        return 1;
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
