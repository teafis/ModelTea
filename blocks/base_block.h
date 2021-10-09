#ifndef BASE_BLOCK_H
#define BASE_BLOCK_H

#include <QGraphicsObject>

#include <string>
#include <cstddef>

class BaseBlock : public QGraphicsObject
{
public:
    BaseBlock(QObject* parent = nullptr);

    virtual void paint(
            QPainter* painter,
            const QStyleOptionGraphicsItem* option,
            QWidget* widget = nullptr);

    virtual QRectF boundingRect() const;

protected:
    void drawIOPorts(
            QPainter* painter,
            bool is_input);

    QRectF blockRect() const;

protected:
    int num_inputs;
    int num_outputs;
    std::string name;
};

#endif // BASE_BLOCK_H
