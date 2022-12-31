// SPDX-License-Identifier: GPL-3.0-only

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>

#include <string>
#include <unordered_map>

class ModelWindow;

class WindowManager : public QObject
{
    Q_OBJECT

public:
    static WindowManager& instance();

private:
    explicit WindowManager(QObject *parent = nullptr);

public:
    size_t next_id() const;

    void register_id(const size_t id, const ModelWindow* window);

    void clear_id(const size_t id);

    bool has_id(const size_t id) const;

    bool model_open(const std::string& n);

private:
    std::unordered_map<size_t, const ModelWindow*> window_id_values;
};

#endif // WINDOWMANAGER_H
