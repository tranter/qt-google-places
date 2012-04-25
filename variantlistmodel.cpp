#include "variantlistmodel.h"

VariantListModel::
VariantListModel(QObject *parent) :
    QAbstractItemModel(parent)
{
}

void VariantListModel::
setData(const QVariantList & data)
{
    beginResetModel();
    m_data = data;
    endResetModel();
}

void VariantListModel::
setData(const QVariant & data)
{
    setData( data.toList() );
}

QVariant VariantListModel::
data(const QModelIndex & index, int role) const
{
    if( role != Qt::DisplayRole ) return QVariant();

    QVariant value = data(index.row());
    return value.toMap()[m_key];
}

QVariant VariantListModel::
data(int row) const
{
    return row < 0 || m_data.count() <= row ? QVariant() : m_data[row];
}

QModelIndex VariantListModel::
index(int row, int column, const QModelIndex & /*parent*/) const
{
    return createIndex(row, column);
}

void VariantListModel::
clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}
