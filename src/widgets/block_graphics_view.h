// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCK_GRAPHICS_VIEW_H
#define BLOCK_GRAPHICS_VIEW_H

#include <QWidget>
#include <QGraphicsView>

#include "blocks/block_object.h"
#include "state/mouse/mouse_state_base.h"

#include <memory>

#include <QPoint>
#include <QGraphicsObject>

#include <tmdl/model.hpp>


class BlockGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    BlockGraphicsView(QWidget* parent = nullptr);

public:
    virtual void mousePressEvent(QMouseEvent* event) override;

    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    virtual void mouseMoveEvent(QMouseEvent* event) override;

    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;

    virtual void changeEvent(QEvent* event) override;

protected:
    virtual QPoint snapMousePositionToGrid(const QPoint& input);

public slots:
    void removeSelectedBlock();

    void updateModel();

signals:
    void modelUpdated();

    void modelChanged();

protected:
    BlockObject* findBlockForMousePress(const QPointF& pos);

    std::optional<BlockObject::PortInformation> findBlockIOForMousePress(
        const QPointF& pos,
        const BlockObject* block);

    bool blockBodyContainsMouse(
        const QPointF& pos,
        const BlockObject* block);

    void addConnectionItem(
        const BlockObject* from_block,
        const size_t from_port,
        const BlockObject* to_block,
        const size_t to_port);

public:
    std::shared_ptr<tmdl::Model> get_model() const;

    void addBlock(std::shared_ptr<tmdl::BlockInterface> blk);

public:
    void onClose();

    std::string getJsonString() const;

    void fromJsonString(const std::string& jsonData);

    bool getChangeFlag() const;

    void resetChangeFlag();

protected:
    std::unique_ptr<MouseStateBase> mouseState;
    QGraphicsObject* selectedItem;
    std::shared_ptr<tmdl::Model> model;
};

#endif // BLOCK_GRAPHICS_VIEW_H
