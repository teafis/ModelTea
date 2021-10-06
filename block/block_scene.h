#ifndef BLOCK_SCENE_H
#define BLOCK_SCENE_H

#include <QGraphicsScene>

#include "test_block.h"

class BlockScene : public QGraphicsScene
{
    Q_OBJECT

public:
    BlockScene(QObject* parent = nullptr);

protected:
    TestBlock block;
};

#endif // BLOCK_SCENE_H
