#ifndef BLOCKGRAPHICSWIDGET_H
#define BLOCKGRAPHICSWIDGET_H

#include <QWidget>
#include <QGraphicsView>

#include "blocks/base_block.h"

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

    virtual void resizeEvent(QResizeEvent* event) override;

protected:
    BaseBlock* findBlockForMousePress(const QPoint& pos);

protected:
    QVector<BaseBlock*> blocks;
};

#endif // BLOCKGRAPHICSWIDGET_H
