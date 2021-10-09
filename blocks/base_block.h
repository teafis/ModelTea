#ifndef BASE_BLOCK_H
#define BASE_BLOCK_H

#include <QGraphicsObject>

#include <string>
#include <cstddef>

class BaseBlock : public QGraphicsObject
{
public:
    BaseBlock(QGraphicsItem* parent = nullptr);

    BaseBlock(QObject* parent = nullptr);

    virtual void paint(
            QPainter* painter,
            const QStyleOptionGraphicsItem* option,
            QWidget* widget = nullptr);

    virtual QRectF boundingRect() const;

protected:
    size_t num_inputs;
    size_t num_outputs;
    std::string name;
};

#endif // BASE_BLOCK_H
