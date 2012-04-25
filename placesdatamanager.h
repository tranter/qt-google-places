#ifndef PLACESDATAMANAGER_H
#define PLACESDATAMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>

class PlacesDataManager : public QObject
{
    Q_OBJECT
    friend class DataManagerHelper;
    friend class DataManagerAutocompleter;
    friend class DataManagerSearch;

public:
    explicit PlacesDataManager(QObject *parent = 0);
    
    void setApiKey(const QString & apikey) { m_strApiKey = apikey; }
    QString getApiKey() const { return m_strApiKey; }

public slots:
    void autocomplete(const QString & apiKey, const QString & input,
                      const QString & location = QString(), const QString & radius = QString(),
                      bool sensor = false);

    void searchPlace(
            const QString & apiKey, const QString & keyword,
            const QString & language, const QString & type,
            const QString & location, const QString & radius,
            bool sensor = false
    );

signals:
    void errorOccured(const QString & error) const;
    void autocompleteData(const QVariant & data);
    void findedPlaces(const QVariant & data);

private slots:
    void replyFinished(QNetworkReply * reply) const;

private:
    void sendRequest( const QString & url, class DataManagerHelper * helper = 0 );
    bool existsReplyError(const QNetworkReply * reply) const;
    
private:
    QNetworkAccessManager m_NetworkAccessManager;

    QString m_strApiKey;
};

#endif // PLACESDATAMANAGER_H
