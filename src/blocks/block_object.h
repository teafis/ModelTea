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
    BlockObject(const std::shared_ptr<tmdl::Block> block);

    virtual void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr);

    virtual QRectF boundingRect() const;

    bool blockRectContainsPoint(const QPointF& localCoords) const;

    const tmdl::Block* get_block() const
    {
        return block.get();
    }

    const BlockIoPort* get_port_for_pos(const QPointF& loc) const;

signals:
    void sceneLocationUpdated();

protected slots:
    void locUpdated()
    {
        emit sceneLocationUpdated();
    }

protected:
    QPointF getIOPortLocation(
        const int number,
        const int io_size,
        const BlockIoPort::PortType type) const;

    void drawIOPorts(
        QPainter* painter,
        const std::vector<std::unique_ptr<BlockIoPort>>& ports);

    QRectF blockRect() const;

protected:
    std::vector<std::unique_ptr<BlockIoPort>> input_ports;
    std::vector<std::unique_ptr<BlockIoPort>> output_ports;

protected:
    std::shared_ptr<tmdl::Block> block;
};

#endif // BASE_BLOCK_H
