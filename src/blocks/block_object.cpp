// SPDX-License-Identifier: GPL-3.0-only

#include "block_object.h"

#include <algorithm>
#include <cmath>

#include <QPainter>
#include <QPalette>
#include <QString>
#include <QWidget>

#include "exceptions/block_object_exception.h"

#include <fmt/format.h>

static const double PADDING_TB = 0;
static const double PADDING_LR = 30;
static const double BASE_SIZE = 50;
static const double HEIGHT_PER_IO = 50;
static const double IO_RADIUS = 5;

BlockObject::BlockObject(const std::shared_ptr<tmdl::BlockInterface> block) : block(block) {
    // Set the provided parent to help with destruction
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Set Position
    setPos(block->get_loc().x, block->get_loc().y);

    // Connect position update values
    connect(this, &BlockObject::xChanged, this, &BlockObject::locUpdated);
    connect(this, &BlockObject::yChanged, this, &BlockObject::locUpdated);
}

void BlockObject::locUpdated() {
    block->set_loc(tmdl::BlockLocation(x(), y()));

    emit sceneLocationUpdated();
}

void BlockObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const auto& p = widget->palette();

    // Draw the block itself
    if (isSelected()) {
        painter->setPen(QPen(p.color(QPalette::Highlight), 3));
    } else {
        painter->setPen(QPen(p.color(QPalette::Text), 1));
    }

    const auto err = block->has_error();

    if (err != nullptr) {
        painter->setBrush(Qt::red);
    } else {
        painter->setBrush(p.color(QPalette::Window));
    }
    painter->drawRect(blockRect());

    painter->setBrush(p.color(QPalette::Window));

    painter->setPen(QPen(p.color(QPalette::Text), 1));

    // Draw the I/O ports
    drawIOPorts(painter, PortType::INPUT);
    drawIOPorts(painter, PortType::OUTPUT);

    // Draw the text label
    QFont font;
    font.setStyleHint(QFont::Courier);
    painter->setFont(font);

    const QFontMetrics font_metrics(font, painter->device());

    // Name For Block
    const std::string name = QString(block->get_name().c_str()).toUpper().toStdString();
    const QString blockName = QString::fromStdString(name.substr(0, static_cast<size_t>(std::min(static_cast<int>(name.size()), 3))));
    const QRect blockNameRect = font_metrics.boundingRect(blockName);

    // Draw the block name
    painter->drawText(boundingRect().center() - QPointF(blockNameRect.width() / 2.0, blockNameRect.height() / 2.0), blockName);
    painter->drawText(boundingRect().center(), QString("%1").arg(block->get_id()));
}

BlockObject::PortLocation BlockObject::getIOPortLocation(const int number, const PortType type) const {
    // Define the lambda used to track X values
    const QRectF rect = boundingRect();
    const double rect_width = rect.width();

    // Define the X and Y offset values
    const double init_x_offset = IO_RADIUS;
    double x_loc = init_x_offset;
    PortSide port_side;

    // Obtain the vector of points
    switch (type) {
    case PortType::INPUT:
        port_side = PortSide::LEFT;
        break;
    case PortType::OUTPUT:
        x_loc = rect_width - x_loc;
        port_side = PortSide::RIGHT;
        break;
    default:
        throw BlockObjectException("unexpected port type provided");
    }

    // Switch if inverted
    if (block->get_inverted()) {
        if (port_side == PortSide::LEFT) {
            port_side = PortSide::RIGHT;
        } else if (port_side == PortSide::RIGHT) {
            port_side = PortSide::LEFT;
        }

        x_loc = rect_width - x_loc;
    }

    // Define the I/O size
    const size_t io_size = getNumPortsForType(type);

    // Ensure that there is at least one port
    if (io_size == 0) {
        throw BlockObjectException("no ports provided");
    }

    // Determine the resulting y location
    double init_y_offset = rect.center().y() - HEIGHT_PER_IO * ((io_size - 1) / 2);
    if (io_size % 2 == 0) {
        init_y_offset += -HEIGHT_PER_IO / 2;
    }

    // Add each line width value, and draw the resulting line
    const double y_loc = init_y_offset + HEIGHT_PER_IO * number;

    // Return the resulting location
    return PortLocation{
        .location = QPointF(x_loc, y_loc),
        .side = port_side,
    };
}

void BlockObject::drawIOPorts(QPainter* painter, const PortType type) {
    const size_t num_ports = getNumPortsForType(type);

    if (num_ports > 0) {
        // Define the lambda used to track X values
        const double rect_width = boundingRect().width();

        const auto x_update_fn = [rect_width, type](const double x) {
            switch (type) {
            case PortType::INPUT:
                return x;
            case PortType::OUTPUT:
                return rect_width - x;
            default:
                throw BlockObjectException("unexpected port type provided");
            }
        };

        // Determine the pen width
        const double pen_width = painter->pen().widthF();

        // Determine the new radius to account for the pen withd
        const double IO_RADIUS_PEN = IO_RADIUS - pen_width;

        // Add each line width value, and draw the resulting line
        for (size_t i = 0; i < num_ports; ++i) {
            const auto loc = getIOPortLocation(i, type).location;

            painter->drawEllipse(loc, IO_RADIUS_PEN, IO_RADIUS_PEN);

            QPointF a(x_update_fn(2 * IO_RADIUS), loc.y());
            QPointF b(x_update_fn(PADDING_LR / 2.0), loc.y());

            painter->drawLine(a, b);
        }
    }
}

QRectF BlockObject::boundingRect() const {
    const int num_inputs_i = static_cast<int>(block->get_num_inputs());
    const int num_outputs_i = static_cast<int>(block->get_num_outputs());

    return QRectF(0, 0, BASE_SIZE + PADDING_LR,
                  BASE_SIZE + PADDING_TB + HEIGHT_PER_IO * std::max({num_inputs_i - 1, num_outputs_i - 1, 0}));
}

QRectF BlockObject::blockRect() const {
    const QRectF bounds = boundingRect();
    return QRectF(bounds.left() + PADDING_LR / 2, bounds.top() + PADDING_TB / 2, bounds.width() - PADDING_LR, bounds.height() - PADDING_TB);
}

bool BlockObject::blockRectContainsPoint(const QPointF& loc) const { return blockRect().contains(loc - pos()); }

void BlockObject::updateBlock() {
    const size_t ITER_LIM = 100;
    size_t i = 0;
    while (block->update_block() && i++ < ITER_LIM)
        ;
}

std::shared_ptr<const tmdl::BlockInterface> BlockObject::get_block() const { return block; }

void BlockObject::setInverted(const bool value) {
    block->set_inverted(value);
    update();
    emit sceneLocationUpdated();
}

bool BlockObject::getInverted() const { return block->get_inverted(); }

QPointF BlockObject::getInputPortLocation(const size_t port_num) const {
    if (port_num < block->get_num_inputs()) {
        return getIOPortLocation(port_num, PortType::INPUT).location;
    } else {
        throw BlockObjectException(fmt::format("provided input port {} is too large for size {}", port_num, block->get_num_inputs()));
    }
}

QPointF BlockObject::getOutputPortLocation(const size_t port_num) const {
    if (port_num < block->get_num_outputs()) {
        return getIOPortLocation(port_num, PortType::OUTPUT).location;
    } else {
        throw BlockObjectException(fmt::format("provided output port {} is too large for size {}", port_num, block->get_num_outputs()));
    }
}

BlockObject::PortSide BlockObject::getInputPortSide(const size_t port_num) const {
    if (port_num < block->get_num_inputs()) {
        return getIOPortLocation(port_num, PortType::INPUT).side;
    } else {
        throw BlockObjectException(fmt::format("provided input port {} is too large for size {}", port_num, block->get_num_inputs()));
    }
}

BlockObject::PortSide BlockObject::getOutputPortSide(const size_t port_num) const {
    if (port_num < block->get_num_outputs()) {
        return getIOPortLocation(port_num, PortType::OUTPUT).side;
    } else {
        throw BlockObjectException(fmt::format("provided output port {} is too large for size {}", port_num, block->get_num_outputs()));
    }
}

size_t BlockObject::getNumPortsForType(const PortType type) const {
    switch (type) {
    case PortType::INPUT:
        return block->get_num_inputs();
    case PortType::OUTPUT:
        return block->get_num_outputs();
    default:
        throw BlockObjectException("unknown port type provided");
    }
}

std::optional<BlockObject::PortInformation> BlockObject::getPortForPosition(const QPointF& loc) const {
    for (const auto& t : {PortType::INPUT, PortType::OUTPUT}) {
        const size_t num_ports = getNumPortsForType(t);

        for (size_t i = 0; i < num_ports; ++i) {
            const auto port_pos = getIOPortLocation(i, t);
            const auto loc_diff = (port_pos.location - (loc - pos()));
            const double radius = std::sqrt(loc_diff.x() * loc_diff.x() + loc_diff.y() * loc_diff.y());

            if (radius < IO_RADIUS) {
                return PortInformation{.block = this, .type = t, .port_count = i};
            }
        }
    }

    return std::nullopt;
}
