#include "placesdatamanager.h"

#include "datamanagerhelper.h"

#include <QNetworkReply>
#include <QApplication>
#include <QUrl>
#include <QDebug>

PlacesDataManager::
PlacesDataManager(QObject *parent) :
    QObject(parent)
{
    connect(& m_NetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void
PlacesDataManager::
sendRequest(const QString & url, DataManagerHelper * helper)
{
    QNetworkRequest request;
    request.setOriginatingObject(helper);

    QUrl percentEncUrl( QUrl::toPercentEncoding(url, ":/?=&,", " ") );

    qDebug() << Q_FUNC_INFO;
    qDebug() << percentEncUrl;

    request.setUrl( percentEncUrl );

    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_NetworkAccessManager.get( request );
}

bool PlacesDataManager::
existsReplyError(const QNetworkReply * reply) const
{
    // Client Error 4xx
    // Server Error 5xx

    if( 399 < reply->error() )
    {
        emit errorOccured( reply->errorString() );
        return true;
    }

    return false;
}

void PlacesDataManager::
replyFinished(QNetworkReply * reply) const
{
    QApplication::restoreOverrideCursor();

    if( existsReplyError(reply) ) return;

    QString strUrl = reply->url().toString();
    QString data = reply->readAll();

    //
    //qDebug() << Q_FUNC_INFO << "\n" << strUrl << "\n" << data;

    QObject * origObject = reply->request().originatingObject();
    if( ! origObject ) {
        qDebug() << "empty originating Object...";
        return;
    }

    DataManagerHelper * helper = qobject_cast<DataManagerHelper*>(origObject);
    if( ! helper ) {
        qDebug() << "empty helper Object...";
        return;
    }

    helper->evalData(data);

    helper->deleteLater();
}

void PlacesDataManager::
autocomplete(
    const QString & apiKey, const QString & input,
    const QString & location, const QString & radius,
    bool sensor
) {
    qDebug() << Q_FUNC_INFO;
    qDebug() << apiKey << input << location << radius << sensor;

    QString url = QString(
        "https://maps.googleapis.com/maps/api/place/autocomplete/json?"
        "key=%1&"
        "input=%2&"
        "sensor=%3"
    ).arg(apiKey, input, sensor ? "true" : "false");

    if( ! location.isEmpty() ) url.append("&location=").append(location);
    if( ! radius.isEmpty() )   url.append("&radius=").append(radius);

    sendRequest(url, new DataManagerAutocompleter(this));
}

void PlacesDataManager::
searchPlace(
    const QString & apiKey, const QString & keyword,
    const QString & language, const QString & types,
    const QString & location, const QString & radius,
    bool sensor
) {
    qDebug() << Q_FUNC_INFO;
    qDebug() << apiKey << types << location << radius << sensor;

    if( location.isEmpty() ) { emit errorOccured("Location is empty"); return; }
    if( radius.isEmpty() )   { emit errorOccured("Radius is empty"); return; }

    QString url = QString(
        "https://maps.googleapis.com/maps/api/place/search/json?"
        "key=%1&"
        "location=%2&"
        "radius=%3&"
        "sensor=%4"
    ).arg(apiKey, location, radius, sensor ? "true" : "false");

    if( ! types.isEmpty() ) url.append("&types=").append(types);
    if( ! keyword.isEmpty() ) url.append("&keyword=").append(keyword);
    if( ! language.isEmpty() ) url.append("&language=").append(language);

    sendRequest(url, new DataManagerSearch(this));
}

