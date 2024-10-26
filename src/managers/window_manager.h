// SPDX-License-Identifier: GPL-3.0-only

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>

#include <string>
#include <unordered_map>

#include <model.hpp>

class ModelWindow;

class WindowManager : public QObject {
    Q_OBJECT

public:
    static WindowManager& instance();

private:
    explicit WindowManager(QObject* parent = nullptr);

public:
    void register_window(const ModelWindow* window, const mtea::Model* model);

    void clear_window(const ModelWindow* window);

    bool model_is_open(const mtea::Model* model) const;

    const ModelWindow* window_for_model(const mtea::Model* model) const;

    std::vector<const ModelWindow*> all_windows();

private:
    std::unordered_map<const ModelWindow*, const mtea::Model*> window_id_values;
};

#endif // WINDOWMANAGER_H
