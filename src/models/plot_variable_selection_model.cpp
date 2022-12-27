#include "plot_variable_selection_model.h"

PlotVariableSelectionModel::PlotVariableSelectionModel(QObject* parent) : QAbstractListModel(parent)
{
    // Empty Constructor
}

int PlotVariableSelectionModel::rowCount(const QModelIndex& parent) const
{
    (void)parent;
    return _items.size();
}

QVariant PlotVariableSelectionModel::data(const QModelIndex& index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        return _items[index.row()].name;
    case Qt::UserRole:
    {
        QVariant msg;
        msg.setValue(_items[index.row()]);
        return msg;
    }
    default:
        return {};
    }
}

void PlotVariableSelectionModel::addItem(const ItemSelector& item)
{
    _items.push_back(item);
}

const QVector<ItemSelector>& PlotVariableSelectionModel::items() const
{
    return _items;
}
