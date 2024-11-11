#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <QString>
#include <QFileInfo>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QList>
#include <QFile>
#include <QDataStream>
#include <QVector>

//#include "sector.h"
#include "res_file.h"
#include "tile_form.h"


class CSector;
class CTile;

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
    void save();
    void saveMapAs(const QFileInfo& path);
    void draw(QOpenGLShaderProgram* program);
    void drawWater(QOpenGLShaderProgram* program);
    void drawTilePreview(QOpenGLShaderProgram* program);
    bool projectPt(QVector3D& point);
    bool projectPt(QVector<QVector3D>& aPoint);
    void projectPositions(QList<CNode*>& aNode);
    void projectPosition(CNode* pNode);
    bool pickTile(QVector3D& point, CTile*& pTileOut, STileLocation& tileLoc, bool bLand = true);
    void setTile(const QMap<STileLocation, STileInfo>& arrTileInfo);
    const QFileInfo& filePath() {return m_filePath;}
    QOpenGLTexture* glTexture() const {return m_texture;}
    uint textureNum() const {return m_header.nTexture;}
    const QVector<SMaterial>& materials() const {return m_aMaterial;}
    void setMaterials(const QVector<SMaterial>& arrMat) {m_aMaterial = arrMat;}
    const QVector<ETileType>& tileTypes() const {return m_aTileTypes;}
    void setTileTypes(const QVector<ETileType>& arrType) {m_aTileTypes = arrType;}
    const QVector<SAnimTile>& animTiles() const {return m_aAnimTile;}
    void setAnimTils(const QVector<SAnimTile>& arrAnimTile) {m_aAnimTile = arrAnimTile;}
    void updateSectorDrawData(int xSec, int ySec);

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
    QVector<int> m_arrIncorectTiles;
    bool m_bDirty;
};



#endif // LANDSCAPE_H
