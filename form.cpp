#include "form.h"
#include "ui_form.h"
#include "placesdatamanager.h"
#include "placesjsmanager.h"
#include "settingsdialog.h"
#include "variantlistmodel.h"
#include "placedetailsdialog.h"
#include "tools.h"
#include "placedialog.h"

#include <QtGui>
#include <QtWebKit>

#include <QJson/QObjectHelper>

/*! \brief Workaround pan/zoom problem with Google Map and QWebView:
 *
 *  http://stackoverflow.com/questions/6184240/map-doesnt-respond-on-mouse-clicks-with-google-maps-api-v3-and-qwebview
 */
class WebPage : public QWebPage
{
public:  WebPage(QObject * p = 0) : QWebPage(p) {}
private: QString userAgentForUrl(const QUrl&) const { return "Chrome/1.0"; }
};



Form::
Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    ui->webView->setPage( new WebPage(this) );
    setupSearchOptionComboboxes();

    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(initMap(bool)));
    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachJsObjects()));

    connect(ui->searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(searchTextChanged(QString)));
    connect(ui->langageComboBox, SIGNAL(textChanged(QString)), this, SLOT(searchTextChanged(QString)));
    connect(ui->placesTypesComboBox, SIGNAL(textChanged(QString)), this, SLOT(searchTextChanged(QString)));

    connect(ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchPlace()));
    connect(ui->autocompleteListView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(autocompleteItemDoubleClicked(QModelIndex)));
    connect(ui->placesListView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(gotoPlace(QModelIndex)));

    m_organizationName = "ICS";
    m_appName = "QtGooglePlaces";

    QSettings settings(m_organizationName, m_appName);
    m_strApiKey = settings.value("apiKey").toString();


    m_pDataManager = new PlacesDataManager(this);
    //m_pDataManager->setApiKey( m_strApiKey );

    m_pJsManager = new PlacesJsManager(this);
    m_pJsManager->setFrame( ui->webView->page()->mainFrame() );

    m_pAutocompletModel = new VariantListModel(this);
    m_pAutocompletModel->setKey("description");
    //m_pSearchCompleter = new QCompleter(this);
    //m_pSearchCompleter->setModel(m_pAutocompletModel);
    //ui->searchLineEdit->setCompleter(m_pSearchCompleter);
    ui->autocompleteListView->setModel(m_pAutocompletModel);

    m_pPlacesModel = new VariantListModel(this);
    m_pPlacesModel->setKey("name");
    ui->placesListView->setModel(m_pPlacesModel);

    attachJsObjects();
    loadHtmlPage( m_strApiKey );

    connect(m_pDataManager, SIGNAL(errorOccured(QString)), this, SLOT(errorOccured(QString)));
    connect(m_pDataManager, SIGNAL(autocompleteData(QVariant)), this, SLOT(autocompleteData(QVariant)));
    connect(m_pDataManager, SIGNAL(findedPlaces(QVariant)), this, SLOT(findedPlaces(QVariant)));
    connect(m_pDataManager, SIGNAL(requestStatus(QString,QVariant)), this, SLOT(requestStatus(QString,QVariant)));

    connect(m_pJsManager, SIGNAL(markerClicked(QString)), this, SLOT(requestPlaceInformation(QString)));
    connect(m_pDataManager, SIGNAL(placeDetails(QVariant)), this, SLOT(showPlaceInformation(QVariant)));

    connect(m_pDataManager, SIGNAL(findCoordinatesByAddress(QString)), this, SLOT(gotoPlaceByCoordinate(QString)));
}

Form::
~Form()
{
    delete ui;
}

void Form::
errorOccured(const QString & error)
{
    QMessageBox::warning(this, trUtf8("Error"), error);
}

void Form::
editSettings()
{
    SettingsDialog dialog(m_organizationName, m_appName, this);

    //if SettingsDialog.Accepted then goto new start location
    if (dialog.exec() == QDialog::Accepted)
    {
        QSettings settings(m_organizationName, m_appName);
        QString location = settings.value("location").toString();

        if(location.isEmpty()) location = "-34.397, 150.644";

        gotoPlaceByCoordinate(location);
    }

}

void Form::
attachJsObjects()
{
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("QtPlaces", m_pJsManager);
}

void Form::
loadHtmlPage(const QString & /*apiKey*/)
{
    qDebug() << Q_FUNC_INFO;

    QFile htmlFile(":/html/index.html");
    if( ! htmlFile.open(QFile::ReadOnly) )
    {
        errorOccured( "I can't read html file" );
        return;
    }

    QTextStream stream( & htmlFile );
    QString html = stream.readAll();

//    int keyPosition = html.indexOf("API_KEY");
//    if( keyPosition < 0 )
//    {
//        errorOccured( "I can't find API_KEY position in html file" );
//        return;
//    }

//    html.remove( keyPosition, 7 ); // "API_KEY"
//    html.insert(keyPosition, apiKey);

    ui->webView->setHtml( html );
}

void Form::
initMap(bool isok)
{
    if( ! isok ) return;

    QSettings settings(m_organizationName, m_appName);
    QString location = settings.value("location").toString();
    if( location.isEmpty() ) location = "-34.397, 150.644";

    ui->webView->page()->mainFrame()->evaluateJavaScript(
        QString("initialize( %1 )").arg( location )
    );
}

void Form::
searchTextChanged(const QString & text)
{
    if( text.isEmpty() ) {
        m_pAutocompletModel->clear();
        return;
    }

    QSettings settings(m_organizationName, m_appName);
    int radius = settings.value("radius").toInt();
    //QString location = settings.value("location").toString();

    QString location = m_pJsManager->getCurrentPointOfView();

    m_pDataManager->autocomplete(m_strApiKey, ui->searchLineEdit->text(), location, ui->langageComboBox->currentText(),
                                 ui->placesTypesComboBox->currentText(), radius, false);
}

void Form::
searchPlace()
{
    QString place = ui->searchLineEdit->text();

    if( place.isEmpty() ) {
        return;
    }

    QSettings settings(m_organizationName, m_appName);
    int radius = settings.value("radius").toInt();
    //QString location = settings.value("location").toString();

    QString location = m_pJsManager->getCurrentPointOfView();

    m_pDataManager->searchPlace(
        m_strApiKey, place,
        ui->langageComboBox->currentText(), ui->placesTypesComboBox->currentText(),
        location, radius
    );

    //m_pJsManager->gotoLocation( location, 9 );
}


void Form::
findedPlaces(const QVariant & data)
{
    m_pPlacesModel->setData(data);
    m_pJsManager->removeMarkers();
    m_pJsManager->createMarkers(data.toList());

//    QSettings settings(m_organizationName, m_appName);
//    QString radius = settings.value("radius").toString();
//    QString location = m_pJsManager->getCurrentPointOfView();

//    m_pJsManager->recreateSearchCircle(location, radius);
}

void Form::
autocompleteData(const QVariant & data)
{
    m_pAutocompletModel->setData(data);
//    ui->autocompleteListView->setCurrentIndex(4);
}

void Form::
autocompleteItemDoubleClicked(const QModelIndex & index)
{
//    ui->searchLineEdit->setText( index.data().toString() );
    m_clickedAddress = index.data().toString();
    m_pDataManager->getCoordinatesByAddress(m_strApiKey, index.data().toString());
}

void Form::
setupSearchOptionComboboxes()
{
    ui->langageComboBox->addItems( Tools::getLanguageList() );
    ui->placesTypesComboBox->addItems( Tools::getTypesList() );
    ui->langageComboBox->setCurrentIndex(-1);
    ui->placesTypesComboBox->setCurrentIndex(-1);
}

void Form::
gotoPlace(const QModelIndex & index)
{
    qDebug() << Q_FUNC_INFO << index.data();
    QVariant data( m_pPlacesModel->data(index.row()) );

    qDebug() << data;

    if( ! data.isValid() ) {
        errorOccured("Empty data");
        return;
    }

    m_pJsManager->gotoPlace( data, 17 );
}

void Form::
requestPlaceInformation(const QString & reference)
{
    m_pDataManager->getPlaceDetails( m_strApiKey, reference, ui->langageComboBox->currentText() );
}

void Form::
showPlaceInformation(const QVariant & info)
{
    QVariantMap infoMap(info.toMap());
    if( infoMap.isEmpty() ) {
        errorOccured( "Returned result is empty" );
        return;
    }
    if( infoMap["status"].toString() != "OK" )  {
        errorOccured( "Returned result status: " + infoMap["status"].toString() );
        return;
    }

    QVariantMap map( info.toMap()["result"].toMap() );

    PlaceDetailsDialog details;
    QJson::QObjectHelper::qvariant2qobject(map, & details);

    if( details.exec() == PlaceDetailsDialog::ToRemove
        &&
            QMessageBox::question(
                this,
                trUtf8("Delete place"),
                trUtf8("Do you really whant to delete place: %1").arg(map["name"].toString()),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No
            ) == QMessageBox::Yes
    ) {
        m_pDataManager->deletePlace(m_strApiKey, details.reference());
    }
}

void Form::
requestStatus(const QString & operation, const QVariant & value)
{
    QString strStatus = value.toMap()["status"].toString();
    bool status = (! value.isValid()) || strStatus != "OK";
    if( status ) {
        errorOccured( QString("Operation: \"%1\" failed\nReturned status: %2").arg(operation, strStatus) );
        return;
    }

    QMessageBox::information(this, operation, QString("Operation: \"%1\" was successful").arg(operation));
}

void Form::
addPlace()
{
    PlaceDialog dialog(this);
    dialog.setLocation( m_pJsManager->getCurrentPointOfView() );
    dialog.setAddress(m_clickedAddress);

    if( dialog.exec() != QDialog::Accepted )
        return;

    m_pDataManager->addPlace(m_strApiKey, dialog.preparedData() );
}

void Form::gotoPlaceByCoordinate(const QString &place)
{
    m_clickedCoordinate = place;
    QString str =
            QString("var newLoc = new google.maps.LatLng(%1); ").arg(place) +
            QString("map.setCenter(newLoc);");
    qDebug() << str;
    ui->webView->page()->currentFrame()->documentElement().evaluateJavaScript(str);
}

