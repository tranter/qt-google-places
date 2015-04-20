#include "placesdatamanager.h"
#include "datamanagerhelper.h"

#include <QJson/Serializer>
#include <QJson/Parser>

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

//    qDebug() << Q_FUNC_INFO;
//    qDebug() << url;
//    qDebug() << "data" << data;

    request.setUrl( url );

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

    //qDebug() << Q_FUNC_INFO << "\n" << reply->operation() <<  strUrl << "\n" << data;

    QObject * origObject = reply->request().originatingObject();

    //qDebug() << Q_FUNC_INFO << "origObject ="  << origObject;


    //it's ok, just simple usage of QNetworkAccessManager, without DataManagerHelper
    if( ! origObject ) {
        //qDebug() << "empty originating Object...";
        //qDebug() << "try simple work with result";
        //qDebug() << data;

        QJson::Parser parser;
        bool ok;
        // json is a QString containing the data to convert

#if QT_VERSION >= 0x050000
        // Qt5 code
        QVariant result = parser.parse (data.toUtf8(), &ok);
#else
        // Qt4 code
        QVariant result = parser.parse (data.toLatin1(), &ok);
#endif


        if(!ok)
        {
            emit errorOccured(QString("Cannot convert to QJson object: %1").arg(data));
            return;
        }

        int code = result.toMap()["Status"].toMap()["code"].toInt();
        if(code != 200 && code != 0)
        {
            emit errorOccured(QString("Code of request is: %1").arg(code));
            return;
        }
        QVariantList placeMarks = result.toMap()["Placemark"].toList();

        //qDebug() << "placeMarks" << placeMarks;

        if (placeMarks.empty())
        {
            //qDebug() << result;
            QVariantList results = result.toMap()["results"].toList();

            //qDebug() << results;


            double east  = results[0].toMap()["geometry"].toMap()["location"].toMap()["lng"].toDouble();
            double north = results[0].toMap()["geometry"].toMap()["location"].toMap()["lat"].toDouble();

            QString str = QString::number(north)+","+QString::number(east);
            emit findCoordinatesByAddress(str);

        }
        return;
    }

    DataManagerHelper * helper = qobject_cast<DataManagerHelper*>(origObject);
    if( ! helper ) {
        //qDebug() << "empty helper Object...";
        return;
    }

    //qDebug() << "Helper object present." ;
    helper->evalData(data);
    helper->deleteLater();
}

void PlacesDataManager::
autocomplete(
    const QString & apiKey, const QString & input,
    const QString & location, const QString & language, const QString & /*type*/,
    const int /*radius*/, bool sensor
) {
    //qDebug() << Q_FUNC_INFO;
    //qDebug() << apiKey << input << location << radius << sensor;

    QString url = QString(
        "https://maps.googleapis.com/maps/api/place/autocomplete/json?"
        "key=%1&"
        "input=%2&"
        "sensor=%3&"
        "language=%4"
    ).arg(apiKey, "\""+input+"\"", sensor ? "true" : "false", language);

    if( ! location.isEmpty() ) url.append("&location=").append(location);

    sendRequest(url, new DataManagerAutocompleter(this));
}

void PlacesDataManager::
searchPlace(
    const QString & apiKey, const QString & keyword,
    const QString & language, const QString & types,
    const QString & location, const int radius,
    bool sensor
) {
    //qDebug() << Q_FUNC_INFO;
    //qDebug() << apiKey << keyword << language<< types<< location << radius << sensor;

    if( location.isEmpty() ) { emit errorOccured("Location is empty"); return; }
    if( radius == 0 )   { emit errorOccured("Radius is empty"); return; }

    QString url = QString(
        "https://maps.googleapis.com/maps/api/place/search/json?"
        "key=%1&"
        "location=%2&"
        "radius=%3&"
        "sensor=%4"
    ).arg(apiKey, location, QString::number(radius), sensor ? "true" : "false");

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

    sendRequest(url, new DataManagerCheckStatus(tr("Deleting place"), this), Post, json.toLatin1());
}

void PlacesDataManager::getCoordinatesByAddress(const QString &apiKey, const QString &address)
{
    qDebug() << "PlacesDataManager::getCoordinatesByAddress";
    QString url = QString("http://maps.google.com/maps/api/geocode/json?address=%1&sensor=false&language=en").arg(address);


    //QString url = QString("http://maps.google.com/maps/geo?q=%1&key=%2&output=json&oe=utf8&sensor=false").arg(address).arg(apiKey);
    m_NetworkAccessManager.get(QNetworkRequest(QUrl(url)));
}

void PlacesDataManager::searchInMapByAddress(const QString &apiKey, const QString &address)
{
    qDebug() << "PlacesDataManager::searchInMapByAddress";
    QString url = QString("http://maps.google.com/maps/api/geocode/json?address=%1&sensor=false&language=en").arg(address);
    //QString url = QString("http://maps.google.com/maps/geo?q=%1&key=%2&output=json&oe=utf8&sensor=false").arg(address).arg(apiKey);
    m_NetworkAccessManager.get(QNetworkRequest(QUrl(url)));
}


void PlacesDataManager::
addEvent(const QString & apiKey, const QVariant & event, bool sensor)
{
    QString url = QString(
        "https://maps.googleapis.com/maps/api/place/event/add/json?"
        "key=%1&"
        "sensor=%2"
    ).arg( apiKey, sensor ? "true" : "false" );

    QJson::Serializer serializer;
    QByteArray json = serializer.serialize(event);

    sendRequest(url, new DataManagerCheckStatus(tr("Adding new event"), this), Post, json);
}

void PlacesDataManager::
deleteEvent(const QString & apiKey, const QString & referenceId, const QString & eventId, bool sensor)
{    
    QString url = QString(
        "https://maps.googleapis.com/maps/api/place/event/delete/json?"
        "key=%1&"
        "sensor=%2"
    ).arg( apiKey, sensor ? "true" : "false" );

    QVariantMap map; map["reference"] = referenceId; map["event_id"] = eventId;
    QByteArray json = QJson::Serializer().serialize(map);

    sendRequest( url, 0, Post, json );
}

//void PlacesDataManager::
//eventDetails(const QString & apiKey, const QString & referenceId, const QString & eventId, bool sensor)
//{
//    Q_UNUSED(apiKey); Q_UNUSED(referenceId); Q_UNUSED(eventId); Q_UNUSED(sensor);
//    //qDebug() << "not implemented";
//}
