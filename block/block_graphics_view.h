#ifndef BLOCKGRAPHICSWIDGET_H
#define BLOCKGRAPHICSWIDGET_H

#include <QWidget>
#include <QGraphicsView>

#include "block_scene.h"

class BlockGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    BlockGraphicsView(QWidget* parent = nullptr);

    virtual void render(
            QPainter *painter,
            const QRectF &target = QRectF(),
            const QRect &source = QRect(),
            Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio);
};

#endif // BLOCKGRAPHICSWIDGET_H
