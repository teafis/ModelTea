// SPDX-License-Identifier: GPL-3.0-only

#ifndef BASE_BLOCK_H
#define BASE_BLOCK_H

#include <QGraphicsObject>

#include "block_io_port.h"

#include <string>
#include <cstddef>


class BaseBlockObject : public QGraphicsObject
{
public:
    BaseBlockObject(QObject* parent = nullptr);

    virtual void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr);

    virtual void updateLocations();

    virtual QRectF boundingRect() const;

protected:
    QPointF getIOPortLocation(
        const int number,
        const int io_size,
        const BlockIoPort::PortType type) const;

    void drawIOPorts(
        QPainter* painter,
        const QVector<BlockIoPort>& ports);

    QRectF blockRect() const;

protected:
    QVector<BlockIoPort> input_ports;
    QVector<BlockIoPort> output_ports;

protected:
    size_t num_inputs;
    size_t num_outputs;
    std::string name;
};

#endif // BASE_BLOCK_H
