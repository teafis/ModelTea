// SPDX-License-Identifier: GPL-3.0-only

#include "executor_manager.h"

#include "exceptions/model_exception.h"

ExecutorManager& ExecutorManager::instance() {
    static ExecutorManager manager;
    return manager;
}

ExecutorManager::ExecutorManager(QObject* parent) : QObject{parent}, _executor_model{} {
    // Empty Constructor
}

std::optional<const tmdl::Model*> ExecutorManager::windowExecutor() const { return _executor_model; }

void ExecutorManager::setWindowExecutor(const tmdl::Model* model) {
    if (_executor_model.has_value()) {
        throw ModelException("model cannot be null");
    }

    _executor_model = model;

    emit executorFlagChanged();
}

void ExecutorManager::reset() {
    _executor_model.reset();

    emit executorFlagChanged();
}
