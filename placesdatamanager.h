#ifndef PLACESDATAMANAGER_H
#define PLACESDATAMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QVariantMap>

class PlacesDataManager : public QObject
{
    Q_OBJECT
    friend class DataManagerHelper;
    friend class DataManagerAutocompleter;
    friend class DataManagerSearch;
    friend class DataManagerPlaceDetails;
    friend class DataManagerCheckStatus;

public:
    explicit PlacesDataManager(QObject *parent = 0);

public slots:
    void autocomplete(
            const QString & apiKey, const QString & input,
            const QString & location = QString(), const QString & language = QString(), const QString & type = QString(),
            const int radius = 0,
            bool sensor = false
    );

    void searchPlace(
            const QString & apiKey, const QString & keyword,
            const QString & language, const QString & type,
            const QString & location, const int radius,
            bool sensor = false
    );

    void getPlaceDetails(
            const QString & apiKey, const QString & reference,
            const QString & language = QString(), bool sensor = false
    );

    void addPlace(const QString & apiKey, const QVariant & place, bool sensor = false);
    void deletePlace(const QString & apiKey, const QString & reference, bool sensor = false);
    void getCoordinatesByAddress(const QString & apiKey, const QString& address);

signals:
    void errorOccured(const QString & error) const;
    void autocompleteData(const QVariant & data);
    void findedPlaces(const QVariant & data);
    void placeDetails(const QVariant & data);
    void requestStatus( const QString & operation, const QVariant & json );
    void findCoordinatesByAddress(const QString & coord) const;


private slots:
    void replyFinished(QNetworkReply * reply) const;

private:
    enum RequestType { Get, Post, Put, Delete };
    void sendRequest(
            const QString & url, class DataManagerHelper * helper = 0,
            RequestType type = Get, const QByteArray & data = QByteArray()
    );
    bool existsReplyError(const QNetworkReply * reply) const;
    
private:
    QNetworkAccessManager m_NetworkAccessManager;
};

#endif // PLACESDATAMANAGER_H
