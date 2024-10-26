// SPDX-License-Identifier: GPL-3.0-only

#ifndef EXECUTORMANAGER_H
#define EXECUTORMANAGER_H

#include <QObject>

#include <optional>

#include <model.hpp>

class ExecutorManager : public QObject {
    Q_OBJECT

public:
    static ExecutorManager& instance();

private:
    explicit ExecutorManager(QObject* parent = nullptr);

public:
    std::optional<const mtea::Model*> windowExecutor() const;

    void setWindowExecutor(const mtea::Model* model);

    void reset();

signals:
    void executorFlagChanged();

private:
    std::optional<const mtea::Model*> _executor_model;
};

#endif // EXECUTORMANAGER_H
