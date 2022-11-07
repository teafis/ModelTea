// SPDX-License-Identifier: GPL-3.0-only

#ifndef BASE_BLOCK_H
#define BASE_BLOCK_H

#include <QGraphicsObject>

#include "block_io_port.h"

#include <string>
#include <cstddef>

#include <memory>

#include <tmdl/block.hpp>


class BlockObject : public QGraphicsObject
{
public:
    BlockObject(
        const std::shared_ptr<tmdl::Block> block,
        QObject* parent = nullptr);

    virtual void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr);

    virtual void updateLocations();

    virtual QRectF boundingRect() const;

    bool blockRectContainsPoint(const QPointF& localCoords) const;

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
    std::shared_ptr<tmdl::Block> block;
};

#endif // BASE_BLOCK_H
