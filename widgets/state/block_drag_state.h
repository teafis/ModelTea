// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCKDRAGSTATE_H
#define BLOCKDRAGSTATE_H

#include <QPointF>

#include "blocks/base_block.h"

class BlockDragState
{
public:
    BlockDragState();

    void reset();

    BaseBlock* getBlock() const;

    const QPointF& getOffset() const;

    bool hasBlock() const;

    void setState(
            const QPointF& inOffset,
            BaseBlock* inBlock);

protected:
    BaseBlock* block;
    QPointF offset;
};

#endif // BLOCKDRAGSTATE_H
