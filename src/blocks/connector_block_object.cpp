// SPDX-License-Identifier: GPL-3.0-only

#include "connector_block_object.h"

#include <QApplication>
#include <QPainter>
#include <QPalette>

#include <cmath>

#include "exceptions/block_object_exception.h"

ConnectorBlockObject::ConnectorBlockObject(std::shared_ptr<tmdl::Connection> connection, const BlockObject* from_block,
                                           const BlockObject* to_block)
    : ConnectorObject(), connection(connection), from_block(from_block), to_block(to_block) {
    // Check Inputs
    if (from_block == nullptr || to_block == nullptr || connection == nullptr) {
        throw BlockObjectException("input parameter cannot be null");
    } else if (connection->get_from_id() != from_block->get_block()->get_id() ||
               connection->get_to_id() != to_block->get_block()->get_id()) {
        throw BlockObjectException("provided connector object does not match connections");
    }

    // Set side values
    side_a = from_block->getOutputPortSide(connection->get_from_port());
    side_b = to_block->getInputPortSide(connection->get_to_port());

    // Set the provided parent to help with destruction
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

bool ConnectorBlockObject::isValidConnection() const {
    if (get_from_port() >= from_block->getNumPortsForType(BlockObject::PortType::OUTPUT)) {
        return false;
    } else if (get_to_port() >= to_block->getNumPortsForType(BlockObject::PortType::INPUT)) {
        return false;
    } else {
        return true;
    }
}

const BlockObject* ConnectorBlockObject::get_from_block() const { return from_block; }

const BlockObject* ConnectorBlockObject::get_to_block() const { return to_block; }

size_t ConnectorBlockObject::get_from_port() const { return connection->get_from_port(); }

size_t ConnectorBlockObject::get_to_port() const { return connection->get_to_port(); }

void ConnectorBlockObject::set_name(const QString s) { connection->set_name(s.toStdString()); }

QString ConnectorBlockObject::get_name() const {
    const auto n = connection->get_name();
    if (n.has_value()) {
        return QString(n->get().c_str());
    } else {
        return "";
    }
}

QRectF ConnectorBlockObject::boundingRect() const {
    const auto pts = getLinePoints();
    float min_x = pts.first().x();
    float max_x = min_x;
    float min_y = pts.first().y();
    float max_y = min_y;

    for (const auto& p : pts) {
        min_x = std::min(min_x, static_cast<float>(p.x()));
        max_x = std::max(max_x, static_cast<float>(p.x()));
        min_y = std::min(min_y, static_cast<float>(p.y()));
        max_y = std::max(max_y, static_cast<float>(p.y()));
    }

    return QRectF(min_x, min_y, max_x - min_x, max_y - min_y);
}

void ConnectorBlockObject::blockLocationUpdated() {
    if (!isValidConnection()) {
        return;
    }

    const auto loc_a_s = to_block->mapToScene(to_block->getInputPortLocation(get_to_port()));
    const auto loc_b_s = from_block->mapToScene(from_block->getOutputPortLocation(get_from_port()));

    side_a = from_block->getOutputPortSide(connection->get_from_port());
    side_b = to_block->getInputPortSide(connection->get_to_port());

    updateLocations(loc_a_s, loc_b_s);
}

QColor ConnectorBlockObject::getLineColor(QWidget* widget) const {
    const auto& p = widget->palette();

    if (isSelected()) {
        return p.color(QPalette::Highlight);
    } else if (!get_name().isEmpty()) {
        return p.color(QPalette::Link);
    } else {
        return p.color(QPalette::Text);
    }
}

double ConnectorBlockObject::getLineWidth() const {
    if (isSelected()) {
        return 5.0;
    } else {
        return 3.0;
    }
}

QVector<QPointF> ConnectorBlockObject::getLinePoints() const {
    if (side_a != side_b) {
        const auto halfway = (loc_a + loc_b) / 2.0;

        return {loc_b, QPointF(halfway.x(), loc_b.y()), QPointF(halfway.x(), loc_a.y()), loc_a};
    } else {
        const float offset_val = 10.0f;
        float offset_x;

        if (side_a == BlockObject::PortSide::RIGHT) {
            offset_x = std::max(loc_a.x(), loc_b.x()) + offset_val;
        } else {
            offset_x = std::min(loc_a.x(), loc_b.x()) - offset_val;
        }

        return {loc_b, QPointF(offset_x, loc_b.y()), QPointF(offset_x, loc_a.y()), loc_a};
    }
}
