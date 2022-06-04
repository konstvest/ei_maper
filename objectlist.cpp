#include <QDebug>
#include <QMessageBox>
#include "objectlist.h"
#include "res_file.h"
#include "utils.h"
#include "view.h"
#include "settings.h"

CObjectList::CObjectList():
    m_pSettings(nullptr)
{
    auto auxFile = QFileInfo(":/auxObjects.res");

    ResFile res(auxFile.filePath());
    QMap<QString, QByteArray> aFile = res.bufferOfFiles();

    for(auto file : aFile.toStdMap())
    {
        if (file.first.toLower().endsWith(".mod"))
            readAssembly(aFile, file.first);
    }
}

CObjectList::~CObjectList()
{
    for (auto fig: m_aFigure)
        fig->~CFigure();
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
    QVector<QFileInfo> fileInfo;
    auto pOpt = dynamic_cast<COptString*>(m_pSettings->opt(eOptSetResource, "figPath1"));
    if (!pOpt || pOpt->value().isEmpty())
    {
        QMessageBox::warning(m_pSettings, "Warning","Choose path to figures.res");
        m_pSettings->onShow(eOptSetResource);
        //todo: wait until user choose resource file and continue
        return;
    }
    else
        fileInfo.append(pOpt->value());

    pOpt = dynamic_cast<COptString*>(m_pSettings->opt(eOptSetResource, "figPath2"));
    if(pOpt && !pOpt->value().isEmpty())
        fileInfo.append(pOpt->value());

    for(auto& file: fileInfo)
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

ei::CFigure* CObjectList::getFigure(const QString& name)
{
    QString figureName = name + ".mod";
    if(!m_aFigure.contains(figureName))
    {
        QSet<QString> figure;
        figure.insert(figureName);
        loadFigures(figure);
    }

    return m_aFigure.contains(figureName) ? m_aFigure[figureName] : figureDefault();
}

ei::CFigure *CObjectList::figureDefault()
{
    Q_ASSERT(m_aFigure.contains("magicTrap.mod"));
    return m_aFigure["magicTrap.mod"];
}
