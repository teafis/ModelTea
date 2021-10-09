#ifndef BASE_BLOCK_H
#define BASE_BLOCK_H

#include <QGraphicsObject>

class BaseBlock : public QGraphicsObject
{
public:
    BaseBlock(QGraphicsItem* parent = nullptr);

    virtual void paint(
            QPainter* painter,
            const QStyleOptionGraphicsItem *option,
            QWidget* widget = nullptr);

    virtual QRectF boundingRect() const;
};

#endif // BASE_BLOCK_H
