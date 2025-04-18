#ifndef PLOTVARIABLESELECTIONMODEL_H
#define PLOTVARIABLESELECTIONMODEL_H

#include <QAbstractListModel>

#include <QtCharts/QLineSeries>
#include <QtCharts>

#include <QVariant>

#include <variable_manager.hpp>

struct ItemSelector {
    std::shared_ptr<const mtea::ModelValue> var;
    QString name;
    QLineSeries* series;
};

Q_DECLARE_METATYPE(ItemSelector)

class PlotVariableSelectionModel : public QAbstractListModel {
    Q_OBJECT

public:
    PlotVariableSelectionModel(QObject* parent = nullptr);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void addItem(const ItemSelector& item);

    const QVector<ItemSelector>& items() const;

protected:
    QVector<ItemSelector> _items;
};

#endif // PLOTVARIABLESELECTIONMODEL_H
