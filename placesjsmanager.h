#ifndef PLACESJSMANAGER_H
#define PLACESJSMANAGER_H

#include <QObject>
#include <QString>

class QWebFrame;

class PlacesJsManager : public QObject
{
    Q_OBJECT
public:
    explicit PlacesJsManager(QObject *parent = 0);
    
    void setFrame(QWebFrame * frame) { m_pFrame = frame; }

    QString getCurrentPointOfView() const;
    
private:
    QWebFrame * m_pFrame;
};

#endif // PLACESJSMANAGER_H
