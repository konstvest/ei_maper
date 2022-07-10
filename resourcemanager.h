#ifndef CRESOURCEMANAGER_H
#define CRESOURCEMANAGER_H

#include <QListWidget>
#include <QFileInfo>
#include <QMap>
#include <QImage>
#include <QOpenGLTexture>
#include "figure.h"

//forward declarations
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

class CTextureList
{
public:
    static CTextureList* getInstance();
    CTextureList(CTextureList const&) = delete;
    void operator=(CTextureList const&)  = delete;

    void loadTexture(QSet<QString>& aName);
    QOpenGLTexture* texture(QString& name);
    QOpenGLTexture* buildLandTex(QString& name, int& texCount);
    QOpenGLTexture* textureDefault();
    void attachSettings(CSettings* pSettings) {m_pSettings = pSettings;};
    void initResource();

private:
    CTextureList();
    ~CTextureList();
    void parse(QByteArray& data, const QString& name);

private:
    static CTextureList* m_pTextureContainer;
    QMap<QString, QOpenGLTexture*> m_aTexture;
    CSettings* m_pSettings;
};

class CResourceManager
{
public:
    CResourceManager();
    void loadResources();
};

#endif // CRESOURCEMANAGER_H
