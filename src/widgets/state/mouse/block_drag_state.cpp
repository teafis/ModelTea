// SPDX-License-Identifier: GPL-3.0-only

#include "block_drag_state.h"

BlockDragState::BlockDragState(BlockObject* block, const QPointF& offset, const QPoint& original)
    : block(block), offset(offset), original(original), current(original) {
    // Empty Constructor
}

BlockObject* BlockDragState::getBlock() const { return block; }

const QPointF& BlockDragState::getOffset() const { return offset; }

const QPoint& BlockDragState::getOriginal() const { return original; }

const QPoint& BlockDragState::getCurrent() const { return current; }

void BlockDragState::setCurrent(const QPoint& p) { current = p; }
