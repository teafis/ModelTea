// SPDX-License-Identifier: GPL-3.0-only

#ifndef SCENE_GRAPHICS_OBJECT_H
#define SCENE_GRAPHICS_OBJECT_H

#include <QGraphicsObject>
#include <QGraphicsScene>

class SceneGraphicsObject : public QGraphicsObject
{
    Q_OBJECT

public slots:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
};

#endif // SCENE_GRAPHICS_OBJECT_H
