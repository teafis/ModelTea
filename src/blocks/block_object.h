// SPDX-License-Identifier: GPL-3.0-only

#ifndef BASE_BLOCK_H
#define BASE_BLOCK_H

#include <QGraphicsObject>

#include <cstddef>
#include <memory>
#include <optional>
#include <string>

#include <tmdl/library.hpp>


class BlockObject : public QGraphicsObject
{
    Q_OBJECT

public:
    BlockObject(const std::shared_ptr<tmdl::LibraryBlock> block);

    virtual void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr);

    virtual QRectF boundingRect() const;

    bool blockRectContainsPoint(const QPointF& localCoords) const;

    void update_block();

    const tmdl::LibraryBlock* get_block() const;

    const QPointF getInputPortLocation(const size_t port_num) const;

    const QPointF getOutputPortLocation(const size_t port_num) const;

public:
    enum class PortType
    {
        OUTPUT = 0,
        INPUT = 1
    };

    struct PortInformation
    {
        const BlockObject* block;
        PortType type;
        size_t port_count;
    };

    size_t getNumPortsForType(const PortType type) const;

    std::optional<PortInformation> get_port_for_pos(const QPointF& loc) const;

signals:
    void sceneLocationUpdated();

protected slots:
    void locUpdated();

protected:
    QPointF getIOPortLocation(
        const int number,
        const PortType type) const;

    void drawIOPorts(
        QPainter* painter,
        const PortType type);

    QRectF blockRect() const;

protected:
    std::shared_ptr<tmdl::LibraryBlock> block;
};

#endif // BASE_BLOCK_H
