// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCKOBJECT_H
#define BLOCKOBJECT_H

#include <QGraphicsObject>

#include <cstddef>
#include <memory>
#include <optional>

#include <block_interface.hpp>

class BlockObject : public QGraphicsObject {
    Q_OBJECT

public:
    enum class PortType {
        OUTPUT = 0,
        INPUT,
    };

    enum class PortSide {
        LEFT = 0,
        RIGHT,
    };

    struct PortInformation {
        const BlockObject* block;
        PortType type;
        size_t port_count;
    };

public:
    BlockObject(const std::shared_ptr<tmdl::BlockInterface> block);

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr);

    virtual QRectF boundingRect() const;

    bool blockRectContainsPoint(const QPointF& localCoords) const;

    void updateBlock();

    std::shared_ptr<const tmdl::BlockInterface> get_block() const;

    void setInverted(bool value);

    bool getInverted() const;

    QPointF getInputPortLocation(const size_t port_num) const;

    QPointF getOutputPortLocation(const size_t port_num) const;

    PortSide getInputPortSide(const size_t port_num) const;

    PortSide getOutputPortSide(const size_t port_num) const;

public:
    size_t getNumPortsForType(const PortType type) const;

    std::optional<PortInformation> getPortForPosition(const QPointF& loc) const;

signals:
    void sceneLocationUpdated();

protected slots:
    void locUpdated();

protected:
    struct PortLocation {
        QPointF location;
        PortSide side;
    };

    PortLocation getIOPortLocation(const int number, const PortType type) const;

    void drawIOPorts(QPainter* painter, const PortType type);

    QRectF blockRect() const;

protected:
    std::shared_ptr<tmdl::BlockInterface> block;
};

#endif // BLOCKOBJECT_H
