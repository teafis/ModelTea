// SPDX-License-Identifier: GPL-3.0-only

#include "block_object.h"

#include <QPainter>
#include <QWidget>

#include <QDebug>

#include <algorithm>

#include <stdexcept>
#include <QString>

#include <QGraphicsObject>

const double PADDING_TB = 0;
const double PADDING_LR = 30;
const double BASE_SIZE = 50;
const double HEIGHT_PER_IO = 50;
const double IO_RADIUS = 5;


BlockObject::BlockObject(
    const std::shared_ptr<tmdl::Block> block,
    QObject* parent) :
    block(block)
{
    // Set the provided parent to help with destruction
    setParent(parent);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    //setFlag(QGraphicsItem::ItemIsMovable, true);
}

void BlockObject::updateLocations()
{
    // Set the locations for the IO ports
    for (size_t i = 0; i < block->get_num_inputs(); ++i)
    {
        input_ports.append(BlockIoPort(
           i,
           getIOPortLocation(
               i,
               block->get_num_inputs(),
               BlockIoPort::PortType::INPUT),
           BlockIoPort::PortType::INPUT));
    }

    for (size_t i = 0; i < block->get_num_outputs(); ++i)
    {
        output_ports.append(BlockIoPort(
           i,
           getIOPortLocation(
               i,
               block->get_num_outputs(),
               BlockIoPort::PortType::OUTPUT),
           BlockIoPort::PortType::OUTPUT));
    }
}

void BlockObject::paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw the block itself
    if (isSelected())
    {
        painter->setPen(QPen(Qt::blue, 2));
    }
    else
    {
        painter->setPen(QPen(Qt::black, 1));
    }
    painter->setBrush(Qt::white);
    painter->drawRect(blockRect());

    painter->setPen(QPen(Qt::black, 1));

    // Draw the I/O ports
    drawIOPorts(
        painter,
        input_ports);
    drawIOPorts(
        painter,
        output_ports);

    // Draw the text label
    QFont font;
    font.setStyleHint(QFont::Courier);
    painter->setFont(font);

    const QFontMetrics font_metrics(font, painter->device());

    // Name For Block
    const std::string name = QString(block->get_name().c_str()).toUpper().toStdString();
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
    painter->drawText(
        boundingRect().center(),
        QString("%1").arg(block->get_id()));
}

QPointF BlockObject::getIOPortLocation(
    const int number,
    const int io_size,
    const BlockIoPort::PortType type) const
{
    // Define the lambda used to track X values
    const QRectF rect = boundingRect();
    const double rect_width = rect.width();

    // Define the X and Y offset values
    const double init_x_offset = IO_RADIUS;
    double x_loc = 0.0;

    // Obtain the vector of points
    switch (type)
    {
    case BlockIoPort::PortType::INPUT:
        x_loc = init_x_offset;
        break;
    case BlockIoPort::PortType::OUTPUT:
        x_loc = rect_width - init_x_offset;
        break;
    default:
        throw std::runtime_error("unexpected port type provided");
    }

    // Ensure that there is at least one port
    if (io_size <= 0)
    {
        throw std::runtime_error("no ports provided");
    }

    // Determine the resulting y location
    double init_y_offset = rect.center().y() - HEIGHT_PER_IO * ((io_size - 1) / 2);
    if (io_size % 2 == 0)
    {
        init_y_offset += -HEIGHT_PER_IO / 2;
    }

    // Add each line width value, and draw the resulting line
    const double y_loc = init_y_offset + HEIGHT_PER_IO * number;

    // Return the resulting location
    return QPointF(
        x_loc,
        y_loc);
}

void BlockObject::drawIOPorts(
    QPainter* painter,
    const QVector<BlockIoPort>& ports)
{
    if (!ports.isEmpty())
    {
        // Define the lambda used to track X values
        const double rect_width = boundingRect().width();

        const auto x_update_fn = [&rect_width](
            const double x,
            const BlockIoPort::PortType type)
        {
            switch (type)
            {
            case BlockIoPort::PortType::INPUT:
                return x;
            case BlockIoPort::PortType::OUTPUT:
                return rect_width - x;
            default:
                throw std::runtime_error("unexpected port type provided");
            }
        };

        // Determine the pen width
        const double pen_width = painter->pen().widthF();

        // Determine the new radius to account for the pen withd
        const double IO_RADIUS_PEN = IO_RADIUS - pen_width;

        // Add each line width value, and draw the resulting line
        for (const BlockIoPort& port : ports)
        {
            const QPointF& loc = port.get_location();

            painter->drawEllipse(
                loc,
                IO_RADIUS_PEN,
                IO_RADIUS_PEN);

            QPointF a(
                x_update_fn(2 * IO_RADIUS, port.get_type()),
                loc.y());
            QPointF b(
                x_update_fn(PADDING_LR / 2.0, port.get_type()),
                loc.y());

            painter->drawLine(a, b);
        }
    }
}

QRectF BlockObject::boundingRect() const
{
    const int num_inputs_i = static_cast<int>(block->get_num_inputs());
    const int num_outputs_i = static_cast<int>(block->get_num_outputs());

    return QRectF(
        0,
        0,
        BASE_SIZE + PADDING_LR,
        BASE_SIZE + PADDING_TB + HEIGHT_PER_IO * std::max({num_inputs_i - 1, num_outputs_i - 1, 0}));
}

QRectF BlockObject::blockRect() const
{
    const QRectF bounds = boundingRect();
    return QRectF(
        bounds.left() + PADDING_LR / 2,
        bounds.top() + PADDING_TB / 2,
        bounds.width() - PADDING_LR,
        bounds.height() - PADDING_TB);
}

bool BlockObject::blockRectContainsPoint(const QPointF& localCoords) const
{
    return blockRect().contains(localCoords);
}
