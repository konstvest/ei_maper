#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <QListWidget>
#include <QFileInfo>
#include "objects\object_base.h"

class CObjectList
{
public:
    CObjectList();
    ~CObjectList();
    //void attachView(CView* view);
    void addResourceFile(const QString& file);
    void addResourceFile(QFileInfo& file);
    void addResourceFile(QVector<QFileInfo>& aFile);
    void loadFigures(QSet<QString>& aFigure);
    void readFigure(const QByteArray& file, const QString& name);
    void readAssembly(const QMap<QString, QByteArray>& aFile, const QString& assemblyRoot);
    //void readFigures(QFileInfo& path);
    ei::CFigure* getFigure(QString& name);

private:
    QVector<QFileInfo> m_aFilePath;
    QMap<QString, ei::CFigure*> m_aFigure;
};

#endif // OBJECTLIST_H
