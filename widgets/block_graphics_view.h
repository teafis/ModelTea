#ifndef BLOCKGRAPHICSWIDGET_H
#define BLOCKGRAPHICSWIDGET_H

#include <QWidget>
#include <QGraphicsView>

#include "blocks/base_block.h"
#include "state/block_drag_state.h"

#include <vector>
#include <QVector>

#include <QPoint>

class BlockGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    BlockGraphicsView(QWidget* parent = nullptr);

public:
    virtual void mousePressEvent(QMouseEvent* event) override;

    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    virtual void mouseMoveEvent(QMouseEvent* event) override;

protected:
    virtual QPoint snapMousePositionToGrid(const QPoint& input);

public slots:
    void addTestBlock();

protected:
    BaseBlock* findBlockForMousePress(const QPointF& pos);

protected:
    QVector<BaseBlock*> blocks;
    BlockDragState mouseDragState;
};

#endif // BLOCKGRAPHICSWIDGET_H
