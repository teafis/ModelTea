// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCKGRAPHICSWIDGET_H
#define BLOCKGRAPHICSWIDGET_H

#include <QWidget>
#include <QGraphicsView>

#include "blocks/base_block.h"
#include "state/block_drag_state.h"

#include <vector>
#include <QVector>

#include <QPoint>

#include <tmdl/model.hpp>

class BlockGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    BlockGraphicsView(QWidget* parent = nullptr);

public:
    virtual void mousePressEvent(QMouseEvent* event) override;

    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    virtual void mouseMoveEvent(QMouseEvent* event) override;

protected:
    virtual QPoint snapMousePositionToGrid(const QPoint& input);

public slots:
    void addTestBlock();

protected:
    BaseBlockObject* findBlockForMousePress(const QPointF& pos);

    BlockIoPort* findBlockIOForMousePress(const QPointF& pos, const BaseBlockObject* block);

    bool blockBodyContainsMouse(const QPointF& pos, const BaseBlockObject* block);

protected:
    QVector<BaseBlockObject*> blocks;
    BlockDragState mouseDragState;
    tmdl::Model model;
};

#endif // BLOCKGRAPHICSWIDGET_H
