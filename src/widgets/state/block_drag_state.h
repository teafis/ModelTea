// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCK_MOUSE_DRAG_STATE_H
#define BLOCK_MOUSE_DRAG_STATE_H

#include <QPointF>

#include "blocks/block_object.h"
#include "mouse_state_base.h"

class BlockDragState : public MouseStateBase
{
public:
    BlockDragState(
        BlockObject* block,
        const QPointF& offset,
        const QPoint& original);

    BlockObject* getBlock() const;

    const QPointF& getOffset() const;

    const QPoint& getOriginal() const;

    const QPoint& getCurrent() const;

    void setCurrent(const QPoint& p);

protected:
    BlockObject* block;
    QPointF offset;
    QPoint original;
    QPoint current;
};

#endif // BLOCK_MOUSE_DRAG_STATE_H
