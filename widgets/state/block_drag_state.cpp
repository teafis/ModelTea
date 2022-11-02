// SPDX-License-Identifier: GPL-3.0-only

#include "block_drag_state.h"

BlockDragState::BlockDragState()
{
    reset();
}

void BlockDragState::reset()
{
    block = nullptr;
    offset = QPointF(0.0, 0.0);
}

BaseBlockObject* BlockDragState::getBlock() const
{
    return block;
}

const QPointF& BlockDragState::getOffset() const
{
    return offset;
}

bool BlockDragState::hasBlock() const
{
    return block != nullptr;
}

void BlockDragState::setState(
        const QPointF& inOffset,
        BaseBlockObject* inBlock)
{
    block = inBlock;
    offset = inOffset;
}
