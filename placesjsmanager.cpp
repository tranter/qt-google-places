#include "placesjsmanager.h"

#include <QWebFrame>
#include <QDebug>
#include <QMessageBox>

#include <qjson/serializer.h>

PlacesJsManager::
PlacesJsManager(QObject *parent) :
    QObject(parent)
{
}

QString PlacesJsManager::
getCurrentPointOfView() const
{
    return eval( "getCurrentPointOfView()" ).toString();
}

QVariant PlacesJsManager::
eval(const QString & script) const
{
    return m_pFrame->evaluateJavaScript( script );
}

void PlacesJsManager::
removeMarkers()
{
    eval( "removeMarkers()" );
}

void PlacesJsManager::
createMarkers(const QVariantList & list)
{
    qDebug() << Q_FUNC_INFO << list.count();

    QJson::Serializer serializer;
    QString json;

    foreach(const QVariant & el, list)
    {
        json = serializer.serialize( el );
        eval( QString("appendMarker(%1)").arg( json ) );
    }
}

void PlacesJsManager::
recreateSearchCircle(const QString & location, const QString & radius)
{
    eval( QString("recreateSearchCircle(%1, %2)").arg(location, radius) );
}
