// SPDX-License-Identifier: GPL-3.0-only

#include "port_drag_state.h"

#include <optional>

PortDragState::PortDragState(const BlockObject::PortInformation& port) :
    connector(nullptr)
{
    add_port(port);

    std::optional<QPointF> startPoint;

    switch (port.type)
    {
    case BlockObject::PortType::INPUT:
        startPoint = port.block->getInputPortLocation(port.port_count);
        break;
    case BlockObject::PortType::OUTPUT:
        startPoint = port.block->getInputPortLocation(port.port_count);
        break;
    }

    if (startPoint)
    {
        connector = new ConnectorObject();
    }
}

PortDragState::~PortDragState()
{
    if (connector)
    {
        connector->deleteLater();
        connector = nullptr;
    }
}

void PortDragState::add_port(const BlockObject::PortInformation& port)
{
    switch (port.type)
    {
    case BlockObject::PortType::INPUT:
        port_input = port;
        break;
    case BlockObject::PortType::OUTPUT:
        port_output = port;
        break;
    default:
        return;
    }
}

bool PortDragState::is_partial() const
{
    return port_output || port_input;
}

bool PortDragState::is_complete() const
{
    return port_output && port_input;
}

const BlockObject::PortInformation& PortDragState::get_output() const
{
    return port_output.value();
}

const BlockObject::PortInformation& PortDragState::get_input() const
{
    return port_input.value();
}

void PortDragState::updateMouseLocation(QPointF loc)
{
    if (connector != nullptr)
    {
        connector->updateLocB(loc);
    }
}

void PortDragState::reset()
{
    port_output = std::nullopt;
    port_input = std::nullopt;
}

ConnectorObject* PortDragState::get_connector() const
{
    return connector;
}
