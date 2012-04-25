#ifndef PLACEDETAILSDIALOG_H
#define PLACEDETAILSDIALOG_H

#include <QDialog>

namespace Ui {
class PlaceDetailsDialog;
}

class PlaceDetailsDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString name                         WRITE setName)
    Q_PROPERTY(QString formatted_address            WRITE setAddress)
    Q_PROPERTY(QString formatted_phone_number       WRITE setPhone)
    Q_PROPERTY(QString international_phone_number   WRITE setInterPhone)
    Q_PROPERTY(QString icon                         WRITE setIcon)
    Q_PROPERTY(QString website                      WRITE setWebsiteUrl)
    
public:
    explicit PlaceDetailsDialog(QWidget *parent = 0);
    ~PlaceDetailsDialog();

    void setName(const QString &);
    void setAddress(const QString &);
    void setPhone(const QString &);
    void setInterPhone(const QString &);
    void setIcon(const QString &);
    void setWebsiteUrl(const QString &);
    
private:
    Ui::PlaceDetailsDialog *ui;
};

#endif // PLACEDETAILSDIALOG_H
