// SPDX-License-Identifier: GPL-3.0-only

#ifndef EXECUTORMANAGER_H
#define EXECUTORMANAGER_H

#include <QObject>

#include <optional>

#include <tmdl/model.hpp>

class ExecutorManager : public QObject {
    Q_OBJECT

public:
    static ExecutorManager& instance();

private:
    explicit ExecutorManager(QObject* parent = nullptr);

public:
    std::optional<const tmdl::Model*> windowExecutor() const;

    void setWindowExecutor(const tmdl::Model* model);

    void reset();

signals:
    void executorFlagChanged();

private:
    std::optional<const tmdl::Model*> _executor_model;
};

#endif // EXECUTORMANAGER_H
