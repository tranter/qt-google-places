#include "tools.h"

#include <QFile>
#include <QTextStream>

QStringList Tools::m_languages;
QStringList Tools::m_types;

QStringList Tools::
readList(const QString & filename)
{
    QStringList list;
    QFile file(filename);
    if( ! file.open(QFile::ReadOnly) )
        return list;

    list.append("");

    QString data = QTextStream(& file).readAll();
    foreach(const QString & entry, data.split("\n"))
    {
        data = entry.trimmed();
        if( data.isEmpty() ) continue;

        list.append( data );
    }

    return list;
}

QStringList Tools::
getLanguageList()
{
    if( m_languages.isEmpty() ) m_languages = readList(":/other/languages.txt");
    return m_languages;
}

QStringList Tools::
getTypesList()
{
    if( m_types.isEmpty() ) m_types = readList(":/other/types.txt");
    return m_types;
}
