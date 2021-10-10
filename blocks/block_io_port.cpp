#include "block_io_port.h"

BlockIoPort::BlockIoPort(
    const int number,
    const QPointF& location,
    const PortType type) :
    location(location),
    number(number),
    type(type)
{
    // Empty Constructor
}

const QPointF& BlockIoPort::get_location() const
{
    return location;
}

int BlockIoPort::get_number() const
{
    return number;
}

BlockIoPort::PortType BlockIoPort::get_type() const
{
    return type;
}
