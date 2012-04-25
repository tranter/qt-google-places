#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "tools.h"

#include <QSettings>

SettingsDialog::SettingsDialog(
        const QString & organizationName,
        const QString & appName,
        QWidget * parent
) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    m_organizationName(organizationName),
    m_appName(appName)
{
    ui->setupUi(this);
    ui->apiKeyLineEdit->setValidator( new QRegExpValidator(QRegExp("\\S+"), this) );
    ui->locationLineEdit->setValidator( new QRegExpValidator( Tools::locationRegExp(), this) );

    QSettings settings(m_organizationName, m_appName);

    ui->apiKeyLineEdit->setText( settings.value("apiKey").toString() );
    ui->locationLineEdit->setText( settings.value("location").toString() );
    ui->radiusSpinBox->setValue( settings.value("radius", 25).toInt() );
    ui->sensorCheckBox->setChecked( settings.value("sensor", false).toBool() );
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::
accept()
{
    QSettings settings(m_organizationName, m_appName);
    settings.setValue("apiKey", ui->apiKeyLineEdit->text());
    settings.setValue("location", ui->locationLineEdit->text());
    settings.setValue("radius", ui->radiusSpinBox->value());
    settings.setValue("sensor", ui->sensorCheckBox->isChecked());

    QDialog::accept();
}
