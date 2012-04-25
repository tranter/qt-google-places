#include "form.h"
#include "ui_form.h"
#include "placesdatamanager.h"
#include "placesjsmanager.h"
#include "settingsdialog.h"
#include "variantlistmodel.h"

#include <QtGui>
#include <QtWebKit>

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
    connect(ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchPlace()));
    connect(ui->autocompleteListView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(autocompleteItemDoubleClicked(QModelIndex)));


    m_organizationName = "ICS";
    m_appName = "QtGooglePlaces";

    QSettings settings(m_organizationName, m_appName);
    m_strApiKey = settings.value("apiKey").toString();


    m_pDataManager = new PlacesDataManager(this);
    m_pDataManager->setApiKey( m_strApiKey );

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
    dialog.exec();
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
    QString radius = settings.value("radius").toString();
    //QString location = settings.value("location").toString();

    QString location = m_pJsManager->getCurrentPointOfView();

    m_pDataManager->autocomplete(m_strApiKey, text, location, radius, false);
}

void Form::
searchPlace()
{
    QString place = ui->searchLineEdit->text();

    if( place.isEmpty() ) {
        return;
    }

    QSettings settings(m_organizationName, m_appName);
    QString radius = settings.value("radius").toString();
    //QString location = settings.value("location").toString();

    QString location = m_pJsManager->getCurrentPointOfView();

    m_pDataManager->searchPlace(
        m_strApiKey, place,
        ui->langageComboBox->currentText(), ui->placesTypesComboBox->currentText(),
        location, radius
    );
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
    //ui->searchComboBox->setCurrentIndex(0);
}

void Form::
autocompleteItemDoubleClicked(const QModelIndex & index)
{
    ui->searchLineEdit->setText( index.data().toString() );
}

void Form::
setupSearchOptionComboboxes()
{
    ui->langageComboBox->addItems(
        QStringList() << "en" << "fr" << "ja" << "pl" << "ru" << "sl"
    );
    ui->placesTypesComboBox->addItems(
        QStringList()
                << "airport" << "bank" << "bar" << "bus_station"
                << "cafe" << "food" << "museum" << "park" << "police" << "zoo"
    );
    ui->langageComboBox->setCurrentIndex(-1);
    ui->placesTypesComboBox->setCurrentIndex(-1);
}
