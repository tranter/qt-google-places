#ifndef PLACEDETAILSDIALOG_H
#define PLACEDETAILSDIALOG_H

#include <QDialog>
#include <QVariantList>

namespace Ui {
class PlaceDetailsDialog;
}

class PlaceDetailsDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString id                           WRITE setId READ id)
    Q_PROPERTY(QString reference                    WRITE setReference READ reference)
    Q_PROPERTY(QString name                         WRITE setName)
    Q_PROPERTY(QString formatted_address            WRITE setAddress)
    Q_PROPERTY(QString formatted_phone_number       WRITE setPhone)
    Q_PROPERTY(QString international_phone_number   WRITE setInterPhone)
    Q_PROPERTY(QString icon                         WRITE setIcon)
    Q_PROPERTY(QString website                      WRITE setWebsiteUrl)
    Q_PROPERTY(QVariant events                      WRITE setEvents)
    
public:
    enum { ToRemove = Rejected + 10, AddEvent, RemoveEvents };

    explicit PlaceDetailsDialog(QWidget *parent = 0);
    ~PlaceDetailsDialog();

    QString id() const { return m_id; }
    QString reference() const { return m_reference; }
    QStringList selectedEvents() const;

    void setId(const QString & v) { m_id = v; }
    void setReference(const QString & v) { m_reference = v; }

    void setName(const QString &);
    void setAddress(const QString &);
    void setPhone(const QString &);
    void setInterPhone(const QString &);
    void setIcon(const QString &);
    void setWebsiteUrl(const QString &);
    void setEvents(const QVariant & events);

private slots:
    void removePlace();
    void addEvent();
    void showEvents();

private:
    Ui::PlaceDetailsDialog *ui;

    QString m_id;
    QString m_reference;

    class VariantListModel * m_pEventModel;
    class QAbstractButton * m_pRemoveEventsButton;
};

#endif // PLACEDETAILSDIALOG_H
