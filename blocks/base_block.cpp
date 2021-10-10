#include "base_block.h"

#include <QPainter>
#include <QWidget>

#include <QDebug>

#include <algorithm>

const double PADDING_TB = 0;
const double PADDING_LR = 30;
const double BASE_SIZE = 50;
const double HEIGHT_PER_IO = 50;
const double IO_RADIUS = 5;

BaseBlock::BaseBlock(QObject* parent) :
    num_inputs(1),
    num_outputs(1),
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
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(Qt::white);
    painter->drawRect(blockRect());

    // Draw the I/O ports
    drawIOPorts(
                painter,
                true);
    drawIOPorts(
                painter,
                false);

    // Draw the text label
    QFont font;
    font.setStyleHint(QFont::Courier);
    painter->setFont(font);

    const QFontMetrics font_metrics(font, painter->device());

    // Name For Block
    const QString blockName = QString::fromStdString(
        name.substr(
            0,
            static_cast<size_t>(std::min(
                static_cast<int>(name.size()),
                3))));
    const QRect blockNameRect = font_metrics.boundingRect(blockName);

    // Draw the block name
    painter->drawText(
                boundingRect().center() - QPointF(
                    blockNameRect.width() / 2.0,
                    blockNameRect.height() / 2.0),
                blockName);
}

void BaseBlock::drawIOPorts(
        QPainter* painter,
        bool is_input)
{
    const auto io_count = (is_input) ? num_inputs : num_outputs;

    if (io_count > 0)
    {
        // Define the lambda used to track X values
        const QRectF rect = boundingRect();
        const double rect_width = rect.width();
        const auto x_update_fn = [&rect_width, &is_input](const double x)
        {
            if (is_input)
            {
                return x;
            }
            else
            {
                return rect_width - x;
            }
        };

        // Determien the pen width
        const double pen_width = painter->pen().widthF();

        // Define the X and Y offset values
        const double init_x_offset = IO_RADIUS + pen_width;

        double init_y_offset = rect.center().y() - HEIGHT_PER_IO * ((io_count - 1) / 2);
        if (io_count % 2 == 0)
        {
            init_y_offset += -HEIGHT_PER_IO / 2;
        }

        // Determine the left/right values for the connecting line
        const double line_x_a = 2 * IO_RADIUS + pen_width;
        const double line_x_b = PADDING_LR / 2.0;

        // Add each line width value, and draw the resulting line
        for (int i = 0; i < io_count; ++i)
        {
            const double y_val = init_y_offset + HEIGHT_PER_IO * i;
            const double x_val = x_update_fn(init_x_offset);

            painter->drawEllipse(
                QPointF(
                    x_val,
                    y_val),
                IO_RADIUS,
                IO_RADIUS);

            painter->drawLine(
                QPointF(
                    x_update_fn(line_x_a),
                    y_val),
                QPointF(
                    x_update_fn(line_x_b),
                    y_val));
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
