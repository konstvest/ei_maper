#include <QFileInfo>

#include "landscape.h"
#include "res_file.h"
#include "utils.h"
#include "view.h"
#include "texturelist.h"
#include "node.h"

CLandscape::CLandscape()
{
    m_aSector.clear();
    m_aAnimTile.clear();
    m_aMaterial.clear();
    m_aTileTypes.clear();
}

CLandscape::~CLandscape()
{
    for (auto& xSec: m_aSector)
    {
        for(auto& ySec: xSec)
            delete ySec;
    }
    m_aSector.clear();
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
    for(uint i(0); i<m_header.nTileType; ++i)
    {
        stream >> type;
        m_aTileTypes.append(type);
    }

    SAnimTile animTile;
    for(uint i(0); i<m_header.nAnimTile; ++i)
    {
        stream >> animTile;
        m_aAnimTile.append(animTile);
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

void CLandscape::readMap(QFileInfo& path)
{
    if (!path.exists())
        return;

    QString filePath = path.filePath();
    QString texname = path.baseName();
    int texCount;
    m_texture = m_parentView->texList()->buildLandTex(texname, texCount);

    ResFile map(filePath);
    QMap<QString, QByteArray> aTmp =  map.bufferOfFiles();  // map contains DIfferentCaseName
    QMap<QString, QByteArray> aComponent;
    for (auto& file: aTmp.toStdMap())
        aComponent.insert(file.first.toLower(), file.second);

    QDataStream mpStream(aComponent[path.baseName().toLower() + ".mp"]);
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
            QDataStream secStream(aComponent.take(path.baseName().toLower() + genSectorSuffix(int(x), int(y)) + ".sec"));
            util::formatStream(secStream);
            CSector* sector = new CSector(secStream, m_header.maxZ, texCount);
            sector->setIndex(secIndex);
            xSec.append(sector);
        }
        m_aSector.append(xSec);
    }
}

void CLandscape::draw(QOpenGLShaderProgram* program)
{
    m_texture->bind(0);
    program->setUniformValue("qt_Texture0", 0);
    for (auto& xSec: m_aSector)
        for(auto& ySec: xSec)
            ySec->draw(program);
}

bool CLandscape::projectPt(QVector3D& point)
{
    int xIndex = int(point.x()/32.0f);
    int yIndex = int(point.y()/32.0f);
    //Q_ASSERT(yIndex < m_aSector.size() && xIndex < m_aSector.first().size());
    if(yIndex < m_aSector.size() && xIndex < m_aSector.first().size())
        m_aSector[yIndex][xIndex]->projectPt(point);
    return false;
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

void CLandscape::projectPositions(QList<CNode*>& aNode)
{
    for(auto& node: aNode)
    {
        if(node->nodeType() == eParticle || node->nodeType() == eLight || node->nodeType() == eSound)
        {
            node->setDrawPosition(node->position());
            continue;
        }
        QVector3D landPos(node->position());
        if(node->nodeType() == eUnit)
            landPos -= node->minPosition();
        projectPt(landPos);
        node->setDrawPosition(landPos);
    }
}
