// SPDX-License-Identifier: GPL-3.0-only

#include "window_manager.h"

#include "../windows/model_window.h"

WindowManager& WindowManager::instance()
{
    static WindowManager inst;
    return inst;
}

WindowManager::WindowManager(QObject *parent)
    : QObject{parent}
{
    // Empty Constructor
}

void WindowManager::register_window(const ModelWindow* window, const tmdl::Model* model)
{
    if (model_is_open(model))
    {
        throw 1;
    }
    else if (window == nullptr || model == nullptr)
    {
        throw 2;
    }

    window_id_values[window] = model;
}

void WindowManager::clear_window(const ModelWindow* window)
{
    const auto it = window_id_values.find(window);
    if (it != window_id_values.end())
    {
        window_id_values.erase(it);
    }
}

bool WindowManager::model_is_open(const tmdl::Model* model) const
{
    for (const auto it : window_id_values)
    {
        if (it.second == model)
        {
            return true;
        }
    }

    return false;
}
