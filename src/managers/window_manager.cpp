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

size_t WindowManager::next_id() const
{
    size_t id = 0;
    for (const auto& it : window_id_values)
    {
        if (it.first > id)
        {
            id = it.first;
        }
    }
    return id + 1;
}

void WindowManager::register_id(const size_t id, const ModelWindow* window)
{
    if (has_id(id))
    {
        throw 1;
    }
    else if (window == nullptr)
    {
        throw 2;
    }

    window_id_values.insert({id, window});
}

void WindowManager::clear_id(const size_t id)
{
    const auto it = window_id_values.find(id);
    if (it != window_id_values.end())
    {
        window_id_values.erase(it);
    }
}

bool WindowManager::has_id(const size_t id) const
{
    return window_id_values.find(id) != window_id_values.end();
}

bool WindowManager::model_open(const std::string& n)
{
    for (const auto& it : window_id_values)
    {
        if (it.second->currentModel().toStdString() == n)
        {
            return true;
        }
    }

    return false;
}
