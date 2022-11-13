// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCKDRAGSTATE_H
#define BLOCKDRAGSTATE_H

#include <QPointF>

#include "blocks/block_object.h"

class BlockDragState
{
public:
    BlockDragState();

    void reset();

    BlockObject* getBlock() const;

    const QPointF& getOffset() const;

    bool hasBlock() const;

    void setState(
        const QPointF& inOffset,
        BlockObject* inBlock);

protected:
    BlockObject* block;
    QPointF offset;
};

#endif // BLOCKDRAGSTATE_H
