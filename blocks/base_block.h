// SPDX-License-Identifier: GPL-3.0-only

#ifndef BASE_BLOCK_H
#define BASE_BLOCK_H

#include <QGraphicsObject>

#include "block_io_port.h"

#include <string>
#include <cstddef>

#include <memory>

#include <sim_math/blocks/base_block.hpp>


class BaseBlockObject : public QGraphicsObject
{
public:
    BaseBlockObject(
        const std::shared_ptr<sim_math::BaseBlock> block,
        QObject* parent = nullptr);

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
    std::shared_ptr<sim_math::BaseBlock> block;
};

#endif // BASE_BLOCK_H
