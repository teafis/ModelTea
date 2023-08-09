#include "scene_graphics_object.h"

QVariant SceneGraphicsObject::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSceneChange && value.canConvert<QGraphicsScene*>())
    {
        setParent(value.value<QGraphicsScene*>());
    }

    return QGraphicsObject::itemChange(change, value);
}
