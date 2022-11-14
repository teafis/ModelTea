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
        BlockObject* inBlock,
        const QPointF& inOffset);

    BlockObject* getBlock() const;

    const QPointF& getOffset() const;

protected:
    BlockObject* block;
    QPointF offset;
};

#endif // BLOCK_MOUSE_DRAG_STATE_H
