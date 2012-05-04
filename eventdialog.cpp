#include "eventdialog.h"
#include "ui_eventdialog.h"

#include "tools.h"

#include <QMessageBox>

EventDialog::
EventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventDialog)
{
    ui->setupUi(this);

    ui->langComboBox->addItems( Tools::getLanguageList() );
}

EventDialog::~EventDialog()
{
    delete ui;
}

QVariantMap EventDialog::
getJSONobject() const
{
    QVariantMap map;
    map["duration"]     = duration();
    map["reference"]    = m_reference;
    map["summary"]      = ui->summaryTextEdit->toPlainText();

    QString lang( ui->langComboBox->currentText() );
    if( ! lang.isEmpty() )
        map["language"] = lang;

    QString url( ui->urlLineEdit->text() );
    if( ! url.isEmpty() )
        map["url"] = url;

    return map;
}

int EventDialog::
duration() const
{
    switch(ui->durationComboBox->currentIndex())
    {
    case 1: return ui->durationSpinBox->value() * 3600;
    case 2: return ui->durationSpinBox->value() * 86400;
    default:;
    }

    return ui->durationSpinBox->value() * 60;
}

void EventDialog::
accept()
{
    if( ui->summaryTextEdit->toPlainText().trimmed().isEmpty() )
    {
        QMessageBox::warning(this, tr("Error"), tr("Summary field is empty"));
        return;
    }
    QDialog::accept();
}
