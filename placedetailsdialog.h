#ifndef PLACEDETAILSDIALOG_H
#define PLACEDETAILSDIALOG_H

#include <QDialog>

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
    
public:
    enum { ToRemove = Rejected + 10 };

    explicit PlaceDetailsDialog(QWidget *parent = 0);
    ~PlaceDetailsDialog();

    QString id() const { return m_id; }
    QString reference() const { return m_reference; }
    void setId(const QString & v) { m_id = v; }
    void setReference(const QString & v) { m_reference = v; }

    void setName(const QString &);
    void setAddress(const QString &);
    void setPhone(const QString &);
    void setInterPhone(const QString &);
    void setIcon(const QString &);
    void setWebsiteUrl(const QString &);

private slots:
    void removePlace();

private:
    Ui::PlaceDetailsDialog *ui;

    QString m_id;
    QString m_reference;
};

#endif // PLACEDETAILSDIALOG_H
