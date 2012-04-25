#include "placedialog.h"
#include "ui_placedialog.h"
#include "tools.h"

#include <QMessageBox>

PlaceDialog::PlaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaceDialog)
{
    ui->setupUi(this);

    ui->languageComboBox->addItems( Tools::getLanguageList() );
    ui->availableListWidget->addItems( Tools::getTypesList() );
    ui->availableListWidget->takeItem(0); // removing empty item

    ui->locationLineEdit->setValidator( new QRegExpValidator(QRegExp("\\d+(\\.\\d+){0,1}\\,\\d+(\\.\\d+){0,1}"), this) );
}

PlaceDialog::~PlaceDialog()
{
    delete ui;
}

void PlaceDialog::
on_addButton_clicked()
{
    QList<QListWidgetItem*> items = ui->availableListWidget->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        item->setSelected(false);
        ui->availableListWidget->takeItem( ui->availableListWidget->row(item) );
        ui->toAddListWidget->addItem( item );
    }
}

void PlaceDialog::
on_removeButton_clicked()
{
    QList<QListWidgetItem*> items = ui->toAddListWidget->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        item->setSelected(false);
        ui->toAddListWidget->takeItem( ui->toAddListWidget->row(item) );
        ui->availableListWidget->addItem( item );
    }
}

void PlaceDialog::
accept()
{
    if( ui->nameLineEdit->text().isEmpty() )
    {
        QMessageBox::warning(this, trUtf8("Error"), "Name is empty...");
        return;
    }
    int pos(0);
    QString text = ui->locationLineEdit->text();
    if( ui->locationLineEdit->validator()->validate( text, pos ) != QValidator::Acceptable )
    {
        QMessageBox::warning(this, trUtf8("Error"), "Location is invalid...");
        return;
    }
    if( ! ui->toAddListWidget->count() )
    {
        QMessageBox::warning(this, trUtf8("Error"), "Type isn't added...");
        return;
    }

    QDialog::accept();
}

void PlaceDialog::
setLocation(const QString & location)
{
    ui->locationLineEdit->setText(location);
}

QVariantMap PlaceDialog::
preparedData()
{
    QVariantMap map;
    QVariantMap location;

    bool ok(true);
    location["lat"] = ui->locationLineEdit->text().split(",")[0].toFloat(& ok);

    if( ! ok ) {
        QMessageBox::warning(this, trUtf8("Error"), "Wrong latitude conversion from string to float...");
        return QVariantMap();
    }

    location["lng"] = ui->locationLineEdit->text().split(",")[1].toFloat(& ok);

    if( ! ok ) {
        QMessageBox::warning(this, trUtf8("Error"), "Wrong longitude conversion from string to float...");
        return QVariantMap();
    }

    map["location"] = location;
    map["name"] = ui->nameLineEdit->text();
    map["accuracy"] = ui->spinBox->value();

    if( ui->languageComboBox->currentIndex() > 0 )
        map["language"] = ui->languageComboBox->currentText();

    QVariantList list;
    for(int i(0); i < ui->toAddListWidget->count(); ++i)
        list.append( ui->toAddListWidget->item(i)->text() );

    map["types"] = list;

    return map;
}
