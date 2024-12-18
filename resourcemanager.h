﻿#ifndef CRESOURCEMANAGER_H
#define CRESOURCEMANAGER_H

#include <QListWidget>
#include <QFileInfo>
#include <QMap>
#include <QImage>
#include <QOpenGLTexture>
#include <QJsonObject>
#include "figure.h"
#include "types.h"

//forward declarations
class CSettings;

///
/// \brief The CObjectList class stores information about the currently read 3D figures from the game resources.
///
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
    CSettings* settings(){Q_ASSERT(m_pSettings); return m_pSettings;}
    void attachSettings(CSettings* pSettings) {m_pSettings = pSettings;};
    QList<QString>& figureList() {return m_arrFigureForComboBox;}

    void initResource();

private:
    CObjectList();
    ~CObjectList();
    ei::CFigure* figureDefault();

private:
    static CObjectList* m_pObjectContainer;
    CSettings* m_pSettings;
    QMap<QString, ei::CFigure*> m_aFigure;
    QList<QString> m_arrFigureForComboBox; //optimization for cell widget
};



// https://www.gipat.ru/forum/index.php?showtopic=3357 - format description

enum ETextureFormat
{
//    DD = 0x00006666
//    ,DXT1 = 0x31545844
//    ,DXT3 = 0x33545844
//    ,PNT3 = 0x33544E50
    eMMP_5650    = 0x5650
    ,eMMP_5550    = 0x5550
    ,eMMP_5551    = 0x5551
    ,eMMP_4444    = 0x4444
    ,eMMP_8888    = 0x8888
    ,eMMP_DXT1    = 0x31545844
    ,eMMP_DXT2    = 0x32545844
    ,eMMP_DXT3    = 0x33545844
    ,eMMP_DXT4    = 0x34545844
    ,eMMP_DXT5    = 0x35545844
    ,eMMP_DXTN    = 0x00545844
    ,eMMP_PAINT   = 0x00544E50
    ,eMMP_PAINT32 = 0x32544E50
    ,eMMP_PNT3    = 0x33544E50
};

struct SMmpHeader
{
    uint m_signature;
    int m_width;
    int m_height;
    int m_mipcount;
    uint m_format;
    int size() {return 76;}

    friend QDataStream& operator >> (QDataStream& data, SMmpHeader& header)
    {
        uint w;
        uint h;
        data >> header.m_signature >> w >> h >> header.m_mipcount >> header.m_format;
        header.m_width = int(w);
        header.m_height = int(h);
        return data;
    }
    friend QDataStream& operator << (QDataStream& data, SMmpHeader& header)
    {
        uint w = uint(header.m_width);
        uint h = uint(header.m_height);
        data << header.m_signature << w << h << header.m_mipcount << header.m_format;
        return data;
    }
};

///
/// \brief The CTextureList class stores information about the currently read textures from the game resources.
///
class CTextureList
{
public:
    static CTextureList* getInstance();
    CTextureList(CTextureList const&) = delete;
    void operator=(CTextureList const&)  = delete;

    void loadTexture(QSet<QString>& aName);
    QOpenGLTexture* texture(const QString& name);
    QOpenGLTexture* buildLandTex(QString& name, int& texCount);
    QOpenGLTexture* textureDefault();
    void attachSettings(CSettings* pSettings) {m_pSettings = pSettings;};
    void initResource();
    const QList<QString>& textureList() const {return m_arrCellComboBox;}
    int extractMmpToDxt1(QVector<QImage>& outArrImage, const QStringList& inArrTextureName);
    int extractMmpToDxt1(QImage& outImage, const QString textureName);

private:
    CTextureList();
    ~CTextureList();
    void parse(QByteArray& data, const QString& name);
    void initAuxTexture();

private:
    static CTextureList* m_pTextureContainer;
    QMap<QString, QOpenGLTexture*> m_aTexture;
    CSettings* m_pSettings;
    QList<QString> m_arrCellComboBox; //optimization for cell widget
};


///
/// \brief The CResourceStringList class manages string localization
///
class CResourceStringList
{
public:
    static CResourceStringList* getInstance();
    CResourceStringList(CResourceStringList const&) = delete;
    void operator=(CResourceStringList const&)  = delete;
    bool getPropList(QMap<uint, QString>& map, const EObjParam propType);
    const QMap<ETileType, QString>& tileTypes() {return m_tileType;}
    const QMap<ETerrainType, QString>& materialType() {return m_materialType;}
    const char* noLiquidIndexName();

private:
    CResourceStringList();
    ~CResourceStringList();
    void initResourceString();

private:
    static CResourceStringList* m_pResourceStringContainer;
    QMap<EObjParam, QMap<uint, QString>> m_propValueName;
    QMap<ETileType, QString> m_tileType;
    QMap<ETerrainType, QString> m_materialType;
};

bool isDIfferent(const QString& value);
QString valueDifferent();


class CNvttManager
{
public:
    CNvttManager(QString appDir);
    CNvttManager() = delete;
    ~CNvttManager() {}
    int dxtToBmp(QString pathToDxt, QString pathToBmp);
    QString bmpFromTexture(QString textureName);

private:
    QString m_nvcompress;
    bool m_bInit;
};

class CSessionDataManager
{
public:
    static CSessionDataManager* getInstance();
    void getLastSession(QString& mprPath, QVector<QString>& arrMobPath);
    void addZoneData(const QString& mprPath, const QVector<QString>& arrMobPath);
    bool getZoneData(QString& mprPath, QVector<QString>& arrMobPath);
    void addCameraData(const QVector3D& position, const QVector3D& pivot, const QVector3D& rotation);
    bool getCameraData(QVector3D& position, QVector3D& pivot, QVector3D& rotation);
    void addQuickTileIndices(const QVector<int>& arrInd);
    bool getdQuickTileIndices(QVector<int>& arrInd);
    void saveSession();
    void reset();
    void loadSession();

private:
    CSessionDataManager();
    ~CSessionDataManager();
    QString sessionDataFile();

private:
    static CSessionDataManager* m_pSessionDataManger;
    QJsonObject m_lastSession;
};

#endif // CRESOURCEMANAGER_H
