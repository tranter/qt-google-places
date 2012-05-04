#include "placedetailsdialog.h"
#include "ui_placedetailsdialog.h"
#include "variantlistmodel.h"
#include "tools.h"

#include <QPushButton>
#include <QListView>

PlaceDetailsDialog::PlaceDetailsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaceDetailsDialog)
{
    ui->setupUi(this);
    ui->eventsLabel->setVisible(false);
    ui->m_pEventList->setVisible(false);

    m_pEventModel = new VariantListModel(ui->m_pEventList);
    ui->m_pEventList->setModel(m_pEventModel);
    m_pEventModel->setKey("summary");

    QPushButton * button = ui->buttonBox->addButton(tr("Remove place"), QDialogButtonBox::NoRole);
    connect(button, SIGNAL(clicked()), this, SLOT(removePlace()));

    button = ui->buttonBox->addButton(tr("Add event"), QDialogButtonBox::NoRole);
    connect(button, SIGNAL(clicked()), this, SLOT(addEvent()));

    m_pRemoveEventsButton = ui->buttonBox->addButton(tr("Remove selected events"), QDialogButtonBox::NoRole);
    connect(m_pRemoveEventsButton, SIGNAL(clicked()), this, SLOT(showEvents()));
    m_pRemoveEventsButton->setVisible(false);
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
    ui->webView->setHtml(
        QString("<html><body style=\"background: #ffffff url('%1') no-repeat center center\"/></body></html>")
        .arg(v)
    );
}

void PlaceDetailsDialog::
setWebsiteUrl(const QString & v)
{
    ui->websiteLabel->setText(
        QString("<html><body><a href=\"%1\">%2</a></body></html>")
        .arg(v, v.left(64)+"...")
    );
}

void PlaceDetailsDialog::
setEvents(const QVariant & jsonEvents)
{
    if( ! jsonEvents.isValid() ) {
        return;
    }

    ui->eventsLabel->setVisible(true);
    ui->m_pEventList->setVisible(true);
    m_pRemoveEventsButton->setVisible(true);

    m_pEventModel->setData(jsonEvents);
}

void PlaceDetailsDialog::
removePlace()
{
    done(ToRemove);
}


void PlaceDetailsDialog::
addEvent()
{
    done(AddEvent);
}

void PlaceDetailsDialog::
showEvents()
{
    done(RemoveEvents);
}

QStringList PlaceDetailsDialog::
selectedEvents() const
{
    QModelIndexList list(ui->m_pEventList->selectionModel()->selectedIndexes());
    QStringList ids; QString id;
    foreach(const QModelIndex & idx, list)
    {
        id = m_pEventModel->data(idx, "event_id").toString();
        if( id.isEmpty() ) continue;
        ids.append( id );
    }

    return ids;
}
