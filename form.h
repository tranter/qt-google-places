#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QModelIndex>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form(QWidget *parent = 0);
    ~Form();
    
private slots:
    void errorOccured(const QString & error);
    void editSettings();

    void attachJsObjects();

    void loadHtmlPage(const QString & apiKey);
    void initMap(bool);

    void searchTextChanged(const QString & text);
    void searchPlace();
    void findedPlaces(const QVariant & data);
    void autocompleteData(const QVariant & data);
    void autocompleteItemDoubleClicked(const QModelIndex &);
    void gotoPlace(const QModelIndex &);

private:
    void setupSearchOptionComboboxes();

private:
    Ui::Form * ui;
    class PlacesDataManager * m_pDataManager;
    class PlacesJsManager * m_pJsManager;

    class VariantListModel * m_pAutocompletModel;
    //class QCompleter * m_pSearchCompleter;
    class VariantListModel * m_pPlacesModel;

    QString m_strApiKey;
    QString m_organizationName;
    QString m_appName;
};

#endif // FORM_H
