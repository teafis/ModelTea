#include "base_block.h"

#include <QPainter>
#include <QWidget>

#include <QDebug>

const double PADDING_TB = 0;
const double PADDING_LR = 30;
const double BASE_SIZE = 50;
const double HEIGHT_PER_IO = 50;
const double IO_RADIUS = 5;

BaseBlock::BaseBlock(QObject* parent) :
    num_inputs(5),
    num_outputs(3),
    name("TEMP")
{
    setParent(parent);
}

void BaseBlock::paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw the block itself
    painter->setPen(QPen(Qt::black));
    painter->setBrush(Qt::gray);
    painter->drawRect(boundingRect());
    painter->drawRect(blockRect());

    // Draw the I/O ports
    drawIOPorts(
                painter,
                true);
    drawIOPorts(
                painter,
                false);
}

void BaseBlock::drawIOPorts(
        QPainter* painter,
        bool is_input)
{
    const auto io_count = (is_input) ? num_inputs : num_outputs;

    if (io_count > 0)
    {
        double init_y_offset = boundingRect().center().y() - HEIGHT_PER_IO * ((io_count - 1) / 2);
        double init_x_offset = IO_RADIUS + painter->pen().widthF();
        if (!is_input)
        {
            init_x_offset = boundingRect().width() - init_x_offset;
        }
        if (io_count % 2 == 0)
        {
            init_y_offset += -HEIGHT_PER_IO / 2;
        }

        for (auto i = 0; i < io_count; ++i)
        {
            painter->drawEllipse(
                        QPointF(
                            init_x_offset,
                            init_y_offset + HEIGHT_PER_IO * i),
                        IO_RADIUS,
                        IO_RADIUS);
        }
    }
}

QRectF BaseBlock::boundingRect() const
{
    return QRectF(
                0,
                0,
                BASE_SIZE + PADDING_LR,
                BASE_SIZE + PADDING_TB + HEIGHT_PER_IO * std::max({num_inputs - 1, num_outputs - 1, 0}));
}

QRectF BaseBlock::blockRect() const
{
    const QRectF bounds = boundingRect();
    return QRectF(
                bounds.left() + PADDING_LR / 2,
                bounds.top() + PADDING_TB / 2,
                bounds.width() - PADDING_LR,
                bounds.height() - PADDING_TB);
}
