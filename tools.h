#ifndef TOOLS_H
#define TOOLS_H

#include <QStringList>

class Tools
{

public:
    static QStringList getLanguageList();
    static QStringList getTypesList();

private:
    static QStringList readList(const QString & filename);

private:
    static QStringList m_languages;
    static QStringList m_types;
};

#endif // TOOLS_H
