#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <QString>
#include <QFileInfo>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QList>
#include <QFile>
#include <QDataStream>
#include <QVector>

//#include "sector.h"
#include "res_file.h"
#include "tile_form.h"


class CSector;



struct SMapHeader
{
    uint   signature;
    float  maxZ;
    uint   nXSector;
    uint   nYSector;
    uint   nTexture;
    uint   textureSize;
    uint   nTile;
    uint   tileSize;
    ushort nMaterial;
    uint   nAnimTile;

    friend QDataStream& operator>> (QDataStream& is, SMapHeader& head)
    {
        return is >> head.signature >> head.maxZ >> head.nXSector >> head.nYSector  >>
                     head.nTexture >> head.textureSize >> head.nTile >> head.tileSize >>
                     head.nMaterial >> head.nAnimTile;
    }

    friend QDataStream& operator<< (QDataStream& os, const SMapHeader& head)
    {
        return os << head.signature << head.maxZ  << head.nXSector << head.nYSector <<
                     head.nTexture << head.textureSize << head.nTile << head.tileSize <<
                     head.nMaterial << head.nAnimTile;
    }
};

class CNode;

///
/// \brief The CLandscape class provides an implementation of the internal game landscape format (*.mpr)
///
class CLandscape
{
    //TODO: change singletone to simple class to provide several landscape loading (in different tabs)
public:
    static CLandscape* getInstance();
    bool isMprLoad() {return !m_aSector.isEmpty();}
    void unloadMpr();
    void readMap(const QFileInfo& path);
    void saveMapAs(const QFileInfo& path);
    void draw(QOpenGLShaderProgram* program);
    void drawWater(QOpenGLShaderProgram* program);
    bool projectPt(QVector3D& point);
    bool projectPt(QVector<QVector3D>& aPoint);
    void projectPositions(QList<CNode*>& aNode);
    void projectPosition(CNode* pNode);
    void pickTile(QVector3D& point, bool bLand = true);
    void setTile(QVector3D& point, bool bLand = true);
    const QFileInfo& filePath() {return m_filePath;}
    void openParams();
    CTileForm* tileForm() {return m_pPropForm;};
    void addTileRotation(int rot);

private:
    CLandscape();
    ~CLandscape();
    bool readHeader(QDataStream& stream);
    bool serializeMpr(const QString& zoneName, CResFile& mprFile);

private:
    static CLandscape* m_pLand;
    SMapHeader m_header;
    QVector<SMaterial> m_aMaterial; //use pointers instead refs?
    QVector<ETileType> m_aTileTypes; //array using tyle type on current map
    QVector<SAnimTile> m_aAnimTile;
    QVector<QVector<CSector*>> m_aSector;
    QOpenGLTexture* m_texture;
    QFileInfo m_filePath;
    CTileForm* m_pPropForm;
};

#endif // LANDSCAPE_H
