#include "placesjsmanager.h"

#include <QWebFrame>

PlacesJsManager::
PlacesJsManager(QObject *parent) :
    QObject(parent)
{
}

QString PlacesJsManager::
getCurrentPointOfView() const
{
    return m_pFrame->evaluateJavaScript( "getCurrentPointOfView()" ).toString();
}
