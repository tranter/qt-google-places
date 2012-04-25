#include "placedetailsdialog.h"
#include "ui_placedetailsdialog.h"

PlaceDetailsDialog::PlaceDetailsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaceDetailsDialog)
{
    ui->setupUi(this);
}

PlaceDetailsDialog::~PlaceDetailsDialog()
{
    delete ui;
}

void PlaceDetailsDialog::
setName(const QString & v)
{
    ui->nameLabel->setText(v);
    setWindowTitle(tr("Place details: ") + v);
}

void PlaceDetailsDialog::
setAddress(const QString & v)
{
    ui->adressLabel->setText(v);
}

void PlaceDetailsDialog::
setPhone(const QString & v)
{
    ui->phoneLabel->setText(v);
}

void PlaceDetailsDialog::
setInterPhone(const QString & v)
{
    ui->internationalLabel->setText(v);
}

void PlaceDetailsDialog::
setIcon(const QString & v)
{
//    QApplication::palette().brush( QPalette::Window ).color().rgb()
    ui->webView->setHtml(
        QString("<html><body style=\"background: #ffffff url('%1') no-repeat center center\"/></body></html>")
        .arg(v)
    );
}

void PlaceDetailsDialog::
setWebsiteUrl(const QString & v)
{
    ui->websiteLabel->setText(
        QString("<html><body><a href=\"%1\">%1</a></body></html>")
        .arg(v)
    );
}
