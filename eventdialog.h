#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QVariantMap>

namespace Ui {
class EventDialog;
}

class EventDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit EventDialog(QWidget *parent = 0);
    ~EventDialog();

    QVariantMap getJSONobject() const;

private:
    int duration() const;

public slots:
    void setReference(const QString & reference) { m_reference = reference; }
    
private:
    Ui::EventDialog *ui;
    QString m_reference;
};

#endif // EVENTDIALOG_H
