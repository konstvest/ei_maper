#include <QDebug>
#include "objectlist.h"
#include "res_file.h"
#include "utils.h"
#include "view.h"

CObjectList::CObjectList()
{
    m_aFilePath.append(QFileInfo(":/auxObjects.res"));
}

CObjectList::~CObjectList()
{
    for (auto fig: m_aFigure)
        fig->~CFigure();
}

void CObjectList::initResourceFile(QFileInfo &file)
{
    Q_ASSERT(file.exists());
    if(!m_aFilePath.contains(file))
        m_aFilePath.append(file);
}

void CObjectList::initResourceFile(QVector<QFileInfo>& aFile)
{
    for(auto& file: aFile)
        initResourceFile(file);
}

void CObjectList::readFigure(const QByteArray& file, const QString& name)
{
    QDataStream stream(file);
    util::formatStream(stream);
    ei::CFigure* fig = new ei::CFigure;
    fig->readData(stream);
    fig->setName(name);
    m_aFigure.insert(name, fig);
    //addItem(name);
}

//read *.mod files, create figure hierarchy with part offsets
//in: aFile, assemblyRoot
void CObjectList::readAssembly(const QMap<QString, QByteArray>& aFile, const QString& assemblyRoot)
{
    ResFile model(aFile[assemblyRoot]);
    QMap<QString, QByteArray> aComponent  = model.bufferOfFiles();
    QDataStream lnkStream(aComponent[assemblyRoot.split(".mod").first()]);
    util::formatStream(lnkStream);
    int nLink;
    lnkStream >> nLink;

    int compLength;
    QVector<char> name;
    QString compName;
    QMap<QString, ei::CFigure*> aParent;
    for (int i(0); i<nLink; ++i)
    {
        lnkStream >> compLength;
        name.resize(compLength);
        lnkStream.readRawData(name.data(), name.size());
        compName = name.data();
        //create node
        QDataStream compStream(aComponent[compName]);
        util::formatStream(compStream);
        ei::CFigure* fig = new ei::CFigure;
        aParent.insert(compName, fig);
        fig->readData(compStream);
        fig->setName(compName);
        lnkStream >> compLength;
        if (compLength == 0)
        { // place root figure to object list
            m_aFigure.insert(assemblyRoot, fig);
            continue;
        }
        name.resize(compLength);
        lnkStream.readRawData(name.data(), name.size());
        compName = name.data();
        aParent[compName]->addChild(fig);
    }

    //.bon file parse here
    QString bonFile (assemblyRoot.split(".mod").first());
    bonFile.append(".bon");
    ResFile position(aFile[bonFile]);
    aComponent  = position.bufferOfFiles();
    for (auto& fig: aParent.values())
    {
        QDataStream bonStream(aComponent[fig->name()]);
        util::formatStream(bonStream);
        fig->readAssemblyOffset(bonStream);
    }
    m_aFigure[assemblyRoot]->applyAssemblyOffset();
}

void CObjectList::loadFigures(QSet<QString>& aFigure)
{
    for(auto& file: m_aFilePath)
    {
        ResFile res(file.filePath());
        QMap<QString, QByteArray> aFile = res.bufferOfFiles();

        for (auto& fig: aFigure)
        {
            if(m_aFigure.contains(fig)) continue;
            if(!aFile.contains(fig)) continue;

            //parse *.mod & *.bon files for assembly
            if(fig.contains(".mod"))
                readAssembly(aFile, fig);
            else if(fig.contains(".fig"))
                readFigure(aFile[fig], fig);
        }
    }
}

ei::CFigure* CObjectList::getFigure(QString& name)
{
    if(!m_aFigure.contains(name))
    {
        QSet<QString> figure;
        figure.insert(name);
        loadFigures(figure);
    }
    Q_ASSERT(m_aFigure.contains(name));
    return m_aFigure[name];
}
