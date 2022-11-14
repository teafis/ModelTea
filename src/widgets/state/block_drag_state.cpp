// SPDX-License-Identifier: GPL-3.0-only

#include "block_drag_state.h"

BlockDragState::BlockDragState(
    BlockObject* inBlock,
    const QPointF& inOffset)
{
    block = inBlock;
    offset = inOffset;
}

BlockObject* BlockDragState::getBlock() const
{
    return block;
}

const QPointF& BlockDragState::getOffset() const
{
    return offset;
}
