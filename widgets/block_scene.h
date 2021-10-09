#ifndef BLOCK_SCENE_H
#define BLOCK_SCENE_H

#include <QGraphicsScene>

#include "blocks/base_block.h"

class BlockScene : public QGraphicsScene
{
    Q_OBJECT

public:
    BlockScene(QObject* parent = nullptr);

protected:
    BaseBlock block;
};

#endif // BLOCK_SCENE_H
