#ifndef TEST_BLOCK_H
#define TEST_BLOCK_H

#include <QGraphicsObject>

class TestBlock : public QGraphicsObject
{
public:
    TestBlock(QGraphicsItem* parent = nullptr);

    virtual void paint(
            QPainter* painter,
            const QStyleOptionGraphicsItem *option,
            QWidget* widget = nullptr);

    virtual QRectF boundingRect() const;
};

#endif // TEST_BLOCK_H
