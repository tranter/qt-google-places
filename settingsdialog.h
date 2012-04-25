#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(const QString & organizationName, const QString & appName, QWidget *parent = 0);
    ~SettingsDialog();
    
private:
    void accept();

private:
    Ui::SettingsDialog *ui;
    QString m_organizationName;
    QString m_appName;
};

#endif // SETTINGSDIALOG_H
