#include "block_scene.h"

BlockScene::BlockScene(QObject* parent) :
    QGraphicsScene(parent)
{
    addItem(&block);
}
