// SPDX-License-Identifier: GPL-3.0-only

#ifndef EXECUTORMANAGER_H
#define EXECUTORMANAGER_H

#include <QObject>

#include <optional>

class ExecutorManager : public QObject
{
    Q_OBJECT

public:
    static ExecutorManager& instance();

private:
    explicit ExecutorManager(QObject* parent = nullptr);

public:
    std::optional<size_t> windowExecutor() const;

    void setWindowExecutor(const size_t wid);

    void reset();

signals:
    void executorFlagChanged();

private:
    std::optional<size_t> _window_id;
};

#endif // EXECUTORMANAGER_H
