#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <QListWidget>
#include <QFileInfo>
#include "objects\object_base.h"

class CSettings;

class CObjectList
{
public:
    static CObjectList* getInstance();
    CObjectList(CObjectList const&) = delete;
    void operator=(CObjectList const&)  = delete;

    void loadFigures(QSet<QString>& aFigure);
    void readFigure(const QByteArray& file, const QString& name);
    void readAssembly(const QMap<QString, QByteArray>& aFile, const QString& assemblyRoot);
    ei::CFigure* getFigure(const QString& name);
    void attachSettings(CSettings* pSettings) {m_pSettings = pSettings;};

private:
    CObjectList();
    ~CObjectList();
    ei::CFigure* figureDefault();

private:
    static CObjectList* m_pObjectContainer;
    CSettings* m_pSettings;
    QMap<QString, ei::CFigure*> m_aFigure;
};

#endif // OBJECTLIST_H
