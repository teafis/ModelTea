// SPDX-License-Identifier: GPL-3.0-only

#include "window_manager.h"

#include "../windows/model_window.h"
#include "exceptions/model_exception.h"

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
        throw ModelException("model already open");
    }
    else if (window == nullptr || model == nullptr)
    {
        throw ModelException("model or window cannot be null");
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

const ModelWindow* WindowManager::window_for_model(const tmdl::Model* model) const
{
    for (const auto& it : window_id_values)
    {
        if (it.second == model)
        {
            return it.first;
        }
    }

    return nullptr;
}

std::vector<const ModelWindow*> WindowManager::all_windows()
{
    std::vector<const ModelWindow*> ws;
    for (const auto& it : window_id_values)
    {
        ws.push_back(it.first);
    }
    return ws;
}
