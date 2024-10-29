#include <QFileInfo>
#include <QRandomGenerator>

#include "landscape.h"
#include "res_file.h"
#include "utils.h"
#include "resourcemanager.h"
#include "node.h"
#include "sector.h"

CLandscape* CLandscape::m_pLand = nullptr;

CLandscape* CLandscape::getInstance()
{
    if(nullptr == m_pLand)
        m_pLand = new CLandscape();
    return m_pLand;
}

void CLandscape::unloadMpr()
{
    for (auto& xSec: m_aSector)
    {
        for(auto& ySec: xSec)
            delete ySec;
    }
    m_aSector.clear();
    m_aMaterial.clear();
    m_aTileTypes.clear();
    m_aAnimTile.clear();
    delete m_pPropForm;
}

CLandscape::CLandscape():
    m_pPropForm(nullptr)
{
    m_aSector.clear();
    m_aAnimTile.clear();
    m_aMaterial.clear();
    m_aTileTypes.clear();
}

CLandscape::~CLandscape()
{
    unloadMpr();
}

bool CLandscape::readHeader(QDataStream& stream)
{
    const uint mapSignature = 0xce4af672;
    stream >> m_header;
    if (m_header.signature != mapSignature)
    {
        Q_ASSERT("Incorrect landscape signature" && false);
        return false;
    }

    SMaterial mat;
    for(uint i(0); i<m_header.nMaterial; ++i)
    {
        stream >> mat;
        m_aMaterial.append(mat);
    }

    int type;
    for(uint i(0); i<m_header.nTile; ++i)
    {
        stream >> type;
        if(type > 15 || type < 0)
        {
            ei::log(eLogWarning, "incorrect tile type at index: " + QString::number(i));
            m_arrIncorectTiles.append(i);
        }
        m_aTileTypes.append((ETileType)type);
    }

    SAnimTile animTile;
    for(uint i(0); i<m_header.nAnimTile; ++i)
    {
        stream >> animTile;
        m_aAnimTile.append(animTile);
    }
    return true;
}

bool CLandscape::serializeMpr(const QString& zoneName, CResFile& mprFile)
{
    QByteArray mpData;
    QDataStream mpStream(&mpData, QIODevice::WriteOnly);
    util::formatStream(mpStream);

    //write map header data (*.mp)
    m_aAnimTile = m_pPropForm->animTile();
    m_aMaterial = m_pPropForm->material();

    m_header.nAnimTile = m_aAnimTile.size();
    m_header.nMaterial = m_aMaterial.size();
    mpStream << m_header;
    for(auto& mat: m_aMaterial)
    {
        mpStream << mat;
    }

    int type;
    for(auto& tileTyle: m_aTileTypes)
    {
        type = int(tileTyle);
        mpStream << type;
    }

    for(auto& animTile: m_aAnimTile)
    {
        mpStream << animTile;
    }
    // end of header data
    mprFile.addFiledata(zoneName + ".mp", mpData);

    for(int row(0); row < m_aSector.size(); ++row)
    {
        for(int col(0); col<m_aSector[row].size(); ++col)
        {
            QByteArray secData = m_aSector[row][col]->serializeSector();
            QString secName = QString("%1%2%3.sec").arg(zoneName).arg(col, 3, 10, QChar('0')).arg(row, 3, 10, QChar('0'));
            mprFile.addFiledata(secName, secData);
        }
    }
    return true;
}

// Generates sector suffix by number: 001002 - sector x:1 y:2
QString genSectorSuffix(int x, int y)
{
    QString name;
    name.append('0');
    if (x < 10)
    {
        name.append('0');
        name.append(QString::number(x));
    }
    else {
        name.append(QString::number(x));
    }
    name.append('0');
    if (y < 10)
    {
        name.append('0');
        name.append(QString::number(y));
    }
    else {
        name.append(QString::number(y));
    }
    return name;
}

void CLandscape::readMap(const QFileInfo& path)
{
    if (!path.exists())
        return;

    m_filePath = path;

    CResFile map(path.filePath());
    QMap<QString, QByteArray> aTmp =  map.bufferOfFiles();  // map contains DIfferentCaseName
    QMap<QString, QByteArray> aComponent;
    QString innerMapName;
    for (auto& file: aTmp.toStdMap())
    {
        QString fName = file.first.toLower();
        aComponent.insert(fName, file.second);
        if(fName.endsWith(".mp"))
            innerMapName = fName.split(".").front(); //todo: dont split multiple dot names (zone.1.gipath.mp)
    }

    int texCount;
    m_texture = CTextureList::getInstance()->buildLandTex(innerMapName, texCount);

    QDataStream mpStream(aComponent[innerMapName + ".mp"]);
    util::formatStream(mpStream);
    if (!readHeader(mpStream))
        return;

    // read sectors
    UI2 secIndex;
    for (uint y(0); y<m_header.nYSector; ++y)
    {
        QVector<CSector*> xSec;
        for (uint x(0); x<m_header.nXSector; ++x)
        {
            secIndex.reset(x, y);
            QDataStream secStream(aComponent.take(innerMapName + genSectorSuffix(int(x), int(y)) + ".sec"));
            util::formatStream(secStream);
            CSector* sector = new CSector(secStream, m_header.maxZ, texCount);
            sector->setIndex(secIndex);
            xSec.append(sector);
            if(sector->existsTileIndices(m_arrIncorectTiles))
            {
                qDebug() << "sector (" << x << "," << y << ")";
            }
        }
        m_aSector.append(xSec);
    }
    m_pPropForm = new CTileForm();
    m_pPropForm->fillTable(path.baseName(), m_header.nTexture);
    m_pPropForm->setTileTypes(m_aTileTypes);
    m_pPropForm->setMaterial(m_aMaterial);
    m_pPropForm->setAnimTile(m_aAnimTile);
}

void CLandscape::saveMapAs(const QFileInfo& path)
{
    // TODO: check if source texture has correct textureSize and tileSize with map header

    //QString filePath = path.filePath();
    //QFile file(filePath);
    QString zoneName(path.completeBaseName());
    CResFile mprFile;
    serializeMpr(zoneName, mprFile);
    QByteArray data = mprFile.generateResData();
    mprFile.saveToFile(path.filePath());
}

void CLandscape::draw(QOpenGLShaderProgram* program)
{
    m_texture->bind(0);
    program->setUniformValue("qt_Texture0", 0);
    for (auto& xSec: m_aSector)
        for(auto& ySec: xSec)
            ySec->draw(program);
}

void CLandscape::drawWater(QOpenGLShaderProgram *program)
{
    m_texture->bind(0);
    program->setUniformValue("qt_Texture0", 0);
    for (auto& xSec: m_aSector)
        for(auto& ySec: xSec)
            ySec->drawWater(program);
}

bool CLandscape::projectPt(QVector3D& point)
{
    int xIndex = int(point.x()/32.0f);
    int yIndex = int(point.y()/32.0f);
    if(yIndex < 0 || yIndex > m_aSector.size()
        || xIndex < 0 || xIndex > m_aSector.first().size())
    {
//        point.setX(0.0f);
//        point.setY(0.0f);
        point.setZ(0.0f);
        return false;
    }
    //Q_ASSERT(yIndex < m_aSector.size() && xIndex < m_aSector.first().size());
    if(yIndex < m_aSector.size() && xIndex < m_aSector.first().size())
        m_aSector[yIndex][xIndex]->projectPt(point);
    return true;
}

bool CLandscape::projectPt(QVector<QVector3D>& aPoint)
{
    int projectedPt(0);
    for(auto& pt: aPoint)
    {
        projectPt(pt);
        ++projectedPt;
    }

    return projectedPt == aPoint.size();
}

void CLandscape::projectPosition(CNode* pNode)
{
    if(!isMprLoad())
        return;

    if(pNode->nodeType() == eParticle || pNode->nodeType() == eLight || pNode->nodeType() == eSound || pNode->nodeType() == ePatrolPoint || pNode->nodeType() == eLookPoint)
    {
        pNode->setDrawPosition(pNode->position());
        return;
    }
    QVector3D landPos(pNode->position());
    if(pNode->nodeType() == eUnit)
        landPos -= pNode->minPosition();
    projectPt(landPos);
    pNode->setDrawPosition(landPos);
}

void CLandscape::pickTile(QVector3D& point, bool bLand)
{
    int xIndex = int(point.x()/32.0f);
    int yIndex = int(point.y()/32.0f);
    point.setZ(-1.0f);
    int row, col;
    if(yIndex < m_aSector.size() && xIndex < m_aSector.first().size())
    {
        if(m_aSector[yIndex][xIndex]->pickTile(row, col, point, bLand))
        {
            if(bLand)
            {
                const CLandTile& tile = m_aSector[yIndex][xIndex]->arrTile()[row][col];
                m_pPropForm->selectTile(tile.tileIndex());
                m_pPropForm->setTileRotation(tile.tileRotation());
            }
            else
            {
                const CWaterTile& tile = m_aSector[yIndex][xIndex]->arrWater()[row][col];
                m_pPropForm->selectTile(tile.tileIndex());
                m_pPropForm->setTileRotation(tile.tileRotation());
                m_pPropForm->setActiveMatIndex(tile.materialIndex());
            }
        }
    }
}

void CLandscape::setTile(QVector3D& point, bool bLand)
{
    QVector<int> indSelected;
    int rot;
    m_pPropForm->getSelectedTile(indSelected, rot); //todo: get material index
    if(indSelected.isEmpty())
        return;
    int xIndex = int(point.x()/32.0f);
    int yIndex = int(point.y()/32.0f);
    point.setZ(-1.0f);

    //int ind = QRandomGenerator::global()->bounded(256); //todo: get selected data from table
    //int rot = QRandomGenerator::global()->bounded(3);
    if(yIndex < m_aSector.size() && xIndex < m_aSector.first().size())
    {
        //m_aSector[yIndex][xIndex]->setTile(point, ind, rot);
        m_aSector[yIndex][xIndex]->setTile(point, indSelected[QRandomGenerator::global()->bounded(indSelected.size())], rot, bLand, 0); // todo: material index

    }
}

void CLandscape::openParams()
{
    m_pPropForm->show();
}

void CLandscape::addTileRotation(int rot)
{
    int curRot = (m_pPropForm->tileRotation() + rot)%4;
    if(curRot < 0)
        curRot = 3;
    m_pPropForm->setTileRotation(curRot);
}

void CLandscape::projectPositions(QList<CNode*>& aNode)
{
    for(auto& node: aNode)
    {
        projectPosition(node);
    }
}
