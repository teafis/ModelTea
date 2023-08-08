// SPDX-License-Identifier: GPL-3.0-only

#include "connector_block_object.h"

#include <QPainter>

#include "exceptions/block_object_exception.h"


ConnectorBlockObject::ConnectorBlockObject(
    std::shared_ptr<tmdl::Connection> connection,
    const BlockObject* from_block,
    const BlockObject* to_block) :
    ConnectorObject(),
    connection(connection),
    from_block(from_block),
    to_block(to_block)
{
    if (from_block == nullptr || to_block == nullptr || connection == nullptr)
    {
        throw BlockObjectException("input parameter cannot be null");
    }
    else if (connection->get_from_id() != from_block->get_block()->get_id() || connection->get_to_id() != to_block->get_block()->get_id())
    {
        throw BlockObjectException("provided connector object does not match connections");
    }

    // Set the provided parent to help with destruction
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

bool ConnectorBlockObject::isValidConnection() const
{
    if (get_from_port() >= from_block->getNumPortsForType(BlockObject::PortType::OUTPUT))
    {
        return false;
    }
    else if (get_to_port() >= to_block->getNumPortsForType(BlockObject::PortType::INPUT))
    {
        return false;
    }
    else
    {
        return true;
    }
}

const BlockObject* ConnectorBlockObject::get_from_block() const
{
    return from_block;
}

const BlockObject* ConnectorBlockObject::get_to_block() const
{
    return to_block;
}

size_t ConnectorBlockObject::get_from_port() const
{
    return connection->get_from_port();
}

size_t ConnectorBlockObject::get_to_port() const
{
    return connection->get_to_port();
}

void ConnectorBlockObject::set_name(const QString s)
{
    connection->set_name(s.toStdString());
}

QString ConnectorBlockObject::get_name() const
{
    return QString(connection->get_name().c_str());
}

void ConnectorBlockObject::blockLocationUpdated()
{
    if (!isValidConnection())
    {
        return;
    }

    const auto loc_a_s = to_block->mapToScene(to_block->getInputPortLocation(get_to_port()));
    const auto loc_b_s = from_block->mapToScene(from_block->getOutputPortLocation(get_from_port()));

    updateLocations(loc_a_s, loc_b_s);
}

QColor ConnectorBlockObject::getLineColor() const
{
    if (isSelected())
    {
        return Qt::blue;
    }
    else if (!get_name().isEmpty())
    {
        return Qt::cyan;
    }
    else
    {
        return Qt::black;
    }
}

double ConnectorBlockObject::getLineWidth() const
{
    if (isSelected())
    {
        return 5.0;
    }
    else
    {
        return 3.0;
    }
}
