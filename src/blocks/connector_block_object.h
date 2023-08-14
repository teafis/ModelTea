// SPDX-License-Identifier: GPL-3.0-only

#ifndef CONNECTOR_BLOCK_OBJECT_H
#define CONNECTOR_BLOCK_OBJECT_H

#include <QGraphicsObject>
#include <memory>

#include "connector_object.h"
#include "block_object.h"

#include <tmdl/model.hpp>

class ConnectorBlockObject : public ConnectorObject
{
    Q_OBJECT

public:
    ConnectorBlockObject(
        std::shared_ptr<tmdl::Connection> connection,
        const BlockObject* from_block,
        const BlockObject* to_block);

    bool isValidConnection() const;

    const BlockObject* get_from_block() const;

    const BlockObject* get_to_block() const;

    size_t get_from_port() const;

    size_t get_to_port() const;

    void set_name(const QString s);

    QString get_name() const;

public slots:
    void blockLocationUpdated();

protected:
    virtual QColor getLineColor(QWidget* widget) const override;

    virtual double getLineWidth() const override;

protected:
    const std::shared_ptr<tmdl::Connection> connection;

    const BlockObject* from_block;
    const BlockObject* to_block;
};

#endif // CONNECTOR_BLOCK_OBJECT_H
