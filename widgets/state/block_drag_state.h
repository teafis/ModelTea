#ifndef BLOCKDRAGSTATE_H
#define BLOCKDRAGSTATE_H

#include <QPointF>
#include <QMouseEvent>

#include "blocks/base_block.h"

class BlockDragState
{
public:
    BlockDragState();

    void reset();

    BaseBlock* getBlock() const;

    const QPointF& getOffset() const;

    bool hasBlock() const;

    void setState(QMouseEvent* event, BaseBlock* inBlock);

protected:
    BaseBlock* block;
    QPointF offset;
};

#endif // BLOCKDRAGSTATE_H
