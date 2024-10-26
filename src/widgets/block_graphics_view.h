// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLOCK_GRAPHICS_VIEW_H
#define BLOCK_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QWidget>

#include "blocks/block_object.h"
#include "blocks/connector_block_object.h"
#include "state/mouse/mouse_state_base.h"

#include <memory>

#include <QGraphicsObject>
#include <QPoint>

#include <model.hpp>
#include <model_block.hpp>

class BlockGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    BlockGraphicsView(QWidget* parent = nullptr);

public:
    virtual void mousePressEvent(QMouseEvent* event) override;

    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    virtual void mouseMoveEvent(QMouseEvent* event) override;

    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;

    virtual void keyPressEvent(QKeyEvent* event) override;

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

    ConnectorBlockObject* findConnectorForMousePress(const QPointF& pos);

    std::optional<BlockObject::PortInformation> findBlockIOForMousePress(const QPointF& pos, const BlockObject* block);

    bool blockBodyContainsMouse(const QPointF& pos, const BlockObject* block);

    void addConnectionItem(const std::shared_ptr<mtea::Connection> connection, const BlockObject* from_block, const BlockObject* to_block);

    void onModelChanged();

public:
    std::shared_ptr<mtea::Model> get_model() const;

    void set_model(std::shared_ptr<mtea::Model> model);

    void addBlock(std::shared_ptr<mtea::BlockInterface> blk);

protected:
    std::unique_ptr<MouseStateBase> mouseState;
    QGraphicsObject* selectedItem;
    std::shared_ptr<mtea::Model> modelInstance;
};

#endif // BLOCK_GRAPHICS_VIEW_H
