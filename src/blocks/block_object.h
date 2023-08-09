// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCKOBJECT_H
#define BLOCKOBJECT_H

#include "scene_graphics_object.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <string>

#include <tmdl/block_interface.hpp>


class BlockObject : public SceneGraphicsObject
{
    Q_OBJECT

public:
    BlockObject(const std::shared_ptr<tmdl::BlockInterface> block);

    virtual void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr);

    virtual QRectF boundingRect() const;

    bool blockRectContainsPoint(const QPointF& localCoords) const;

    void update_block();

    std::shared_ptr<const tmdl::BlockInterface> get_block() const;

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
    std::shared_ptr<tmdl::BlockInterface> block;
};

#endif // BLOCKOBJECT_H
