#ifndef BLOCK_IO_PORT_H
#define BLOCK_IO_PORT_H

#include <QPointF>

class BlockIoPort
{
public:
    enum class PortType
    {
        INPUT = 0,
        OUTPUT = 1
    };

public:
    BlockIoPort(
        const int number,
        const QPointF& location,
        const PortType type);

    const QPointF& get_location() const;

    const void set_location(const QPointF& loc);

    int get_number() const;

    PortType get_type() const;

protected:
    QPointF location;
    int number;
    PortType type;
};

#endif // BLOCK_IO_PORT_H
