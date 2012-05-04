#ifndef VariantListMODEL_H
#define VariantListMODEL_H

#include <QVariantList>
#include <QAbstractItemModel>

class VariantListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit VariantListModel(QObject *parent = 0);
    
    void setData(const QVariantList & data);
    void setData(const QVariant & data);

    int rowCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent) return m_data.count(); }
    int columnCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent) return 1; }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant data(int row) const;
    QVariant data(const QModelIndex &index, const QString & key) const;

    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child = QModelIndex()) const { Q_UNUSED(child) return QModelIndex(); }

    void clear();

    void setKey(const QString & key) { m_key = key; }

private:
    QString m_key;
    QVariantList m_data;
};

#endif // VariantListMODEL_H
