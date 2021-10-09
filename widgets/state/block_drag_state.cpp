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

BaseBlock* BlockDragState::getBlock() const
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
        BaseBlock* inBlock)
{
    block = inBlock;
    offset = inOffset;
}
