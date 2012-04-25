#ifndef DATAMANAGERHELPER_H
#define DATAMANAGERHELPER_H

#include <QObject>

class PlacesDataManager;

class DataManagerHelper : public QObject
{
    Q_OBJECT

    friend class PlacesDataManager;

protected:
    DataManagerHelper( PlacesDataManager * p = 0) :
        QObject((QObject*)p),
        manager(p)
    { }

    virtual void evalData(const QVariant & data) = 0;

    QVariant parseJsonData(const QVariant & data);

    PlacesDataManager * manager;
};


class DataManagerAutocompleter : public DataManagerHelper
{
    void evalData(const QVariant & data);

public:
    DataManagerAutocompleter( PlacesDataManager * p = 0) :
        DataManagerHelper(p)
    { }
};


class DataManagerSearch : public DataManagerHelper
{
    void evalData(const QVariant & data);

public:
    DataManagerSearch( PlacesDataManager * p = 0) :
        DataManagerHelper(p)
    { }
};


class DataManagerPlaceDetails : public DataManagerHelper
{
    void evalData(const QVariant & data);

public:
    DataManagerPlaceDetails( PlacesDataManager * p = 0) :
        DataManagerHelper(p)
    { }
};

class DataManagerCheckStatus : public DataManagerHelper
{
    void evalData(const QVariant & data);
    QString m_operation;

public:
    DataManagerCheckStatus( const QString & operation, PlacesDataManager * p = 0) :
        DataManagerHelper(p),
        m_operation(operation)
    { }
};

#endif // DATAMANAGERHELPER_H
