// SPDX-License-Identifier: GPL-3.0-only

#include "executor_manager.h"

ExecutorManager& ExecutorManager::instance()
{
    static ExecutorManager manager;
    return manager;
}

ExecutorManager::ExecutorManager(QObject *parent) :
    QObject{parent},
    _window_id{}
{
    // Empty Constructor
}

std::optional<size_t> ExecutorManager::windowExecutor() const
{
    return _window_id;
}

void ExecutorManager::setWindowExecutor(const size_t wid)
{
    if (_window_id.has_value())
    {
        throw 3;
    }

    _window_id = wid;

    emit executorFlagChanged();
}

void ExecutorManager::reset()
{
    _window_id.reset();

    emit executorFlagChanged();
}
