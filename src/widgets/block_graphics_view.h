// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCKGRAPHICSWIDGET_H
#define BLOCKGRAPHICSWIDGET_H

#include <QWidget>
#include <QGraphicsView>

#include "blocks/block_object.h"
#include "state/mouse_state_base.h"

#include <memory>

#include <QPoint>

#include <tmdl/model.hpp>

#include "windows/block_library.h"

class BlockGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    BlockGraphicsView(QWidget* parent = nullptr);

public:
    virtual void mousePressEvent(QMouseEvent* event) override;

    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    virtual void mouseMoveEvent(QMouseEvent* event) override;

    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;

protected:
    virtual QPoint snapMousePositionToGrid(const QPoint& input);

    virtual void keyPressEvent(QKeyEvent *event) override;

public slots:
    void removeSelectedBlock();

    void updateModel();

    void generateExecutor();

    void stepExecutor();

    void clearExecutor();

    void showLibrary();

    void showPlot();

    void addBlock(QString s);

protected slots:
    void libraryClosed();

signals:
    void plotPointUpdated(const double t, const double y);

protected:
    BlockObject* findBlockForMousePress(const QPointF& pos);

    std::optional<BlockObject::PortInformation> findBlockIOForMousePress(
        const QPointF& pos,
        const BlockObject* block);

    bool blockBodyContainsMouse(
        const QPointF& pos,
        const BlockObject* block);

protected:
    std::unique_ptr<MouseStateBase> mouseState;
    BlockObject* selectedBlock;
    tmdl::Model model;

    struct ExecutionState
    {
        std::shared_ptr<tmdl::VariableManager> variables;
        std::shared_ptr<tmdl::BlockExecutionInterface> model;
        tmdl::SimState state;
    };

    std::shared_ptr<tmdl::LibraryBase> library;
    std::unique_ptr<ExecutionState> executor;

    BlockLibrary* libraryWindow;
};

#endif // BLOCKGRAPHICSWIDGET_H
