#ifndef PLACEDIALOG_H
#define PLACEDIALOG_H

#include <QDialog>
#include <QVariantMap>

namespace Ui {
class PlaceDialog;
}

class PlaceDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PlaceDialog(QWidget *parent = 0);
    ~PlaceDialog();

    void setLocation(const QString & location);
    void setAddress(const QString & address);
    QVariantMap preparedData();
    
private slots:
    void on_addButton_clicked();
    void on_removeButton_clicked();

private:
    void accept();

private:
    Ui::PlaceDialog *ui;
};

#endif // PLACEDIALOG_H
