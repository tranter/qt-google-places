#include "placesdatamanager.h"
#include "datamanagerhelper.h"

#include <QJson/Serializer>

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
sendRequest(const QString & url, DataManagerHelper * helper, RequestType type, const QByteArray & data)
{
    QNetworkRequest request;
    request.setOriginatingObject(helper);

    QUrl percentEncUrl( QUrl::toPercentEncoding(url, ":/?=&,", " ") );

    qDebug() << '\n' << Q_FUNC_INFO;
    qDebug() << percentEncUrl;
    qDebug() << data;

    request.setUrl( percentEncUrl );

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if( type == Post || type == Put )
    {
        request.setRawHeader("Content-Type", "application/json");
        //request.setRawHeader("Content-Length", QString::number(data.size()).toLatin1());
        //request.setRawHeader("Host", "maps.googleapis.com");
    }

    switch(type)
    {
    case Get:       m_NetworkAccessManager.get( request ); break;
    case Post:      m_NetworkAccessManager.post( request, data ); break;
    case Put:       m_NetworkAccessManager.put( request, data ); break;
    case Delete:    m_NetworkAccessManager.deleteResource( request ); break;
    }
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
    qDebug() << Q_FUNC_INFO << "\n" << reply->operation() <<  strUrl << "\n" << data;

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
    qDebug() << apiKey << keyword << language<< types<< location << radius << sensor;

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

void PlacesDataManager::
getPlaceDetails( const QString & apiKey, const QString & reference, const QString & language, bool sensor )
{
    QString url = QString(
        "https://maps.googleapis.com/maps/api/place/details/json?"
        "key=%1&"
        "reference=%2&"
        "sensor=%3"
    ).arg(apiKey, reference, sensor ? "true" : "false");

    if( ! language.isEmpty() ) url.append("&language=").append(language);

    sendRequest(url, new DataManagerPlaceDetails(this));
}

void PlacesDataManager::
addPlace(const QString & apiKey, const QVariant & place, bool sensor )
{
    QString url = QString(
        "https://maps.googleapis.com/maps/api/place/add/json?"
        "key=%1&"
        "sensor=%2"
    ).arg( apiKey, sensor ? "true" : "false" );

    QJson::Serializer serializer;
    QByteArray json = serializer.serialize(place);

    sendRequest(url, new DataManagerCheckStatus(tr("Adding new place"), this), Post, json);
}

void PlacesDataManager::
deletePlace(const QString & apiKey, const QString & reference, bool sensor)
{
    QString url = QString(
        "https://maps.googleapis.com/maps/api/place/delete/json?"
        "key=%1&"
        "sensor=%2"
    ).arg( apiKey, sensor ? "true" : "false" );

    QString json = QString("{ \"reference\": \"%1\" }").arg(reference);
    sendRequest(url, new DataManagerCheckStatus(tr("Deleting place"), this), Delete, json.toLatin1());
}
