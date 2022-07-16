#include "resourcemanager.h"
#include <QDebug>
#include <QMessageBox>
#include "res_file.h"
#include "utils.h"
#include "view.h"
#include "types.h"
#include "settings.h"
#include "log.h"

CObjectList* CObjectList::m_pObjectContainer = nullptr;

CObjectList *CObjectList::getInstance()
{
    if(nullptr == m_pObjectContainer)
        m_pObjectContainer = new CObjectList();
    return m_pObjectContainer;
}

CObjectList::CObjectList():
    m_pSettings(nullptr)
{
    //read figures for aux objects.
    //TODO: can be conflict with user model name. load aux figures in separate map
    auto auxFile = QFileInfo(":/auxData.res");

    ResFile res(auxFile.filePath());
    QMap<QString, QByteArray> aFile = res.bufferOfFiles();

    for(auto file : aFile.toStdMap())
    {
        if (file.first.toLower().endsWith(".mod"))
            readAssembly(aFile, file.first);
    }
    ei::log(eLogInfo, "aux objects loaded");
}

CObjectList::~CObjectList()
{
    ei::CFigure* pFig = nullptr;
    foreach (pFig, m_aFigure)
        pFig->~CFigure();
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

    uint n(0);
    QString figName;
    for(auto& file: fileInfo)
    {
        ResFile res(file.filePath());
        QMap<QString, QByteArray> aFile = res.bufferOfFiles();
        if(aFigure.isEmpty())
        {
            QRegExp rx("(infa\\S+face)|(init(ar|we|li|qi|qu)\\S*\\d+(armor|weapon|item))"); //TODO: remove this if figure will load faster
            for (auto& fig : aFile.toStdMap())
            {
                figName = fig.first.split(".")[0];
                if(rx.exactMatch(figName)) continue;

                if(m_aFigure.contains(fig.first)) continue;
                if(fig.first.contains(".mod"))
                    readAssembly(aFile, fig.first);
                else if(fig.first.contains(".fig"))
                    readFigure(aFile[fig.second], fig.first);
                else //bon, lnk files
                    continue;

                m_arrFigureForComboBox.append(figName);
                ++n;
            }
        }

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
    std::sort(m_arrFigureForComboBox.begin(), m_arrFigureForComboBox.end());
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

void CObjectList::initResource()
{
    QSet<QString> empty;
    loadFigures(empty);
}

ei::CFigure *CObjectList::figureDefault()
{
    Q_ASSERT(m_aFigure.contains("cannotDisplay.mod"));
    return m_aFigure["cannotDisplay.mod"];
}



CTextureList* CTextureList::m_pTextureContainer = nullptr;

CTextureList *CTextureList::getInstance()
{
    if(nullptr == m_pTextureContainer)
        m_pTextureContainer = new CTextureList();
    return m_pTextureContainer;
}

CTextureList::CTextureList():
    m_pSettings(nullptr)
{
}

CTextureList::~CTextureList()
{
//    for(auto& tex : m_aTexture)
//    {
//      //error with destroying :(
//        tex->destroy();
//    }
    m_aTexture.clear();
}



uchar extractColor(uint pixel, uint mask, uint shift)
{
    return uchar(0.5 + 255 * ((pixel & mask) >> shift) / (mask >> shift));
}

SColor getColor(uint pixel, SMmpColor& color)
{
    uchar a = color.alpha.m_shift ? extractColor(pixel, color.alpha.m_value, color.alpha.m_mask) : 255;
    uchar r = extractColor(pixel, color.red.m_value, color.red.m_mask);
    uchar g = extractColor(pixel, color.green.m_value, color.green.m_mask);
    uchar b = extractColor(pixel, color.blue.m_value, color.blue.m_mask);

    return SColor(r,g,b,a);
}

SColor getPixelData(QDataStream &stream, uint pixelSize, SMmpColor& color)
{
    SColor colPix;
    if(pixelSize == 2)
    {
        ushort pixel;
        stream >> pixel;
        colPix = getColor(pixel, color);

    }
    else if(pixelSize == 4)
    {
        uint pixel;
        stream >> pixel;
        colPix = getColor(pixel, color);
    }
    return colPix;
}

void decompressPnt3(QImage& image, QDataStream& data, uint mipCount)
{
    const uint offset = 76;
    QByteArray destination;

    uint src = 0;
    uint n = 0;
    uint v;
    while(src < mipCount)
    {
        data.device()->seek(offset + src);
        data >> v;
        src += 4;

        if(v > 1000000 || v == 0)
        {
            n += 1;
        }
        else
        {
            data.device()->seek(offset + (src - (1 + n) * 4));
            destination.append(data.device()->read(src-4));
            for(uint i(0); i< v; i++)
                destination.append('\x00');
            n = 0;
        }
    }

    data.device()->seek(offset + (src - n * 4));
    destination.append(data.device()->read(src));

    n = 0;
    for(int h(0); h < image.height(); ++h)
    {
        for(int w(0); w < image.width(); ++w)
        {
            image.setPixelColor(h, w, QColor(uchar(destination[n + 2]), uchar(destination[n + 1]), uchar(destination[n]), uchar(destination[n + 3])));
            n += 4;
        }
    }

    //rotate image by 270 degree
    QPoint center = image.rect().center();
    QMatrix matrix;
    matrix.translate(center.x(), center.y());
    matrix.rotate(270);
    image = image.transformed(matrix).mirrored();
}


void readDirecDrawImage(QImage& image, QDataStream& stream)
{
    uint pixelSize;
    SColor col;
    SMmpColor color;
    stream >> pixelSize >> color;
    pixelSize /= 8;

    for(int h(0); h < image.height(); ++h)
        for(int w(0); w < image.width(); ++w)
        {
            col = getPixelData(stream, pixelSize, color);
            image.setPixelColor(w,h, QColor(col.red(), col.green(), col.blue(), col.alpha()));
        }
}

//in. data - texture byte data
//in. name - texture name
void CTextureList::parse(QByteArray& data, const QString& name)
{
    QDataStream stream(data);
    util::formatStream(stream);

    SMmpHeader header;
    stream >> header;
    if(header.m_signature != 0x00504D4D)
    {
        Q_ASSERT("incorrect texture signature" && false);
        return;
    }

    QOpenGLTexture* texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture->setMinificationFilter(QOpenGLTexture::NearestMipMapNearest);
    texture->setMagnificationFilter(QOpenGLTexture::NearestMipMapNearest);
    //texture->setMipLevels(4); //this code will generate mipmaps for textures
    //TODO: read mipmaps
//    for (int i(0); i<mipMapsCount; ++i)
//    {
//        texture->setCompressedData(i, 0, size, data.data() + header.size());
//    }
    texture->setWrapMode(QOpenGLTexture::Repeat);
    switch (header.m_format)
    {
    case ETextureFormat::eMMP_DXT1:
    {
        //TODO: read mipmaps
        texture->setFormat(QOpenGLTexture::TextureFormat::RGBA_DXT1);
        texture->setSize(int(header.m_width), int(header.m_height));
        texture->allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
        const int size = int(header.m_width * header.m_height)/2;
        texture->setCompressedData(0, 0, size, data.data() + header.size());
        break;
    }
    case ETextureFormat::eMMP_DXT3:
    {
        //TODO: read mipmaps
        texture->setFormat(QOpenGLTexture::TextureFormat::RGBA_DXT3);
        texture->setSize(int(header.m_width), int(header.m_height));
        texture->allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
        const int size = int(header.m_width * header.m_height);
        texture->setCompressedData(0, 0, size, data.data() + header.size());
        break;
    }
    case ETextureFormat::eMMP_5650    : // use the same direct draw reading algorithm
    case ETextureFormat::eMMP_5551    : // use the same direct draw reading algorithm
    case ETextureFormat::eMMP_4444    : // use the same direct draw reading algorithm
    case ETextureFormat::eMMP_8888    : // use the same direct draw reading algorithm
    {
        QImage img(header.m_width, header.m_height, QImage::Format_RGBA64);
        readDirecDrawImage(img, stream);
        texture->setData(img);
        break;
    }
    case ETextureFormat::eMMP_PNT3    :
    {
        // very slow. 50-100ms
        QImage img(header.m_width, header.m_height, QImage::Format_RGBA64);
        decompressPnt3(img, stream, uint(header.m_mipcount));
        texture->setData(img);
        break;
    }
    case ETextureFormat::eMMP_5550    :qDebug() << "eMMP_5550"  << name; return; // not found
    case ETextureFormat::eMMP_DXT2    :qDebug() << "eMMP_DXT2"  << name; return; // not found
    case ETextureFormat::eMMP_DXT4    :qDebug() << "eMMP_DXT4"  << name; return; // not found
    case ETextureFormat::eMMP_DXT5    :qDebug() << "eMMP_DXT5"  << name; return; // not found
    case ETextureFormat::eMMP_DXTN    :qDebug() << "eMMP_DXTN"  << name; return; // not found
    case ETextureFormat::eMMP_PAINT   :qDebug() << "eMMP_PAINT" << name; return; // not found
    case ETextureFormat::eMMP_PAINT32 :qDebug() << "eMMP_PNT32" << name; return; // not found
    default:
    {
        Q_ASSERT("unknown texture format" && false);
        return;
    }
    }
    m_aTexture.insert(name, texture);
}

void CTextureList::initAuxTexture()
{
    //read textures for aux objects.
    //TODO: can be conflict with user tex name. load aux textures in separate map
    auto auxFile = QFileInfo(":/auxData.res");

    ResFile res(auxFile.filePath());
    QMap<QString, QByteArray> aFile = res.bufferOfFiles();

    QString texName;
    for (auto& packedTex : aFile.toStdMap())
    {
        if(!packedTex.first.toLower().endsWith(".mmp")) continue;
        texName = packedTex.first.split(".")[0];
        if(m_aTexture.contains(texName)) continue;
        parse(packedTex.second, texName);
    }
    ei::log(eLogInfo, "aux texture loaded");
}

void CTextureList::loadTexture(QSet<QString>& aName)
{
    QVector<QFileInfo> fileInfo;
    auto pOpt = dynamic_cast<COptString*>(m_pSettings->opt(eOptSetResource, "texPath1"));
    if (!pOpt || pOpt->value().isEmpty())
    {
        QMessageBox::warning(m_pSettings, "Warning", "Choose path to textures.res");
        m_pSettings->onShow(eOptSetResource);
        //todo: wait until user choose resource file and continue
        return;
    }
    else
        fileInfo.append(pOpt->value());

    pOpt = dynamic_cast<COptString*>(m_pSettings->opt(eOptSetResource, "texPath2"));
    if(pOpt && !pOpt->value().isEmpty())
        fileInfo.append(pOpt->value());

    QString texName;

    uint n(0);
    for(auto& file: fileInfo)
    {
        ResFile res(file.filePath());
        QMap<QString, QByteArray> aFile = res.bufferOfFiles();
        if (aName.isEmpty())
        {

            //load all textures exclude "special"
            QRegExp rx("((material|spell|modifier|prototype|qitem|quitem|litem|skill)\\d{2,4})|(^\\S+\\d{3})|(_\\d{2}\\.\\d)|(face\\S*\\d+\\S*)|((sm_)?cursor_\\S+)|(zone\\S+(questm?|map))|(un(mo|un)\\S+w[1-3])"); //TODO: delete this when loading textures will be faster...
            rx.setCaseSensitivity(Qt::CaseInsensitive);
            for (auto& packedTex : aFile.toStdMap())
            {
                texName = packedTex.first.split(".")[0];
                if(rx.exactMatch(texName)) continue;
                if(m_aTexture.contains(texName)) continue;
                //if(packedTex.first.toLower().contains("zone")) continue;
                parse(packedTex.second, texName);
                m_arrCellComboBox.append(texName);
                ++n;
            }

        }
        else
            for(auto& name: aName)
            {
                //if(!name.contains(".mmp")) continue;
                if(m_aTexture.contains(name)) continue;
                if(!aFile.contains(name + ".mmp")) continue;

                parse(aFile[name + ".mmp"], name);
            }
    }
    std::sort(m_arrCellComboBox.begin(), m_arrCellComboBox.end());
}

QOpenGLTexture* CTextureList::texture(const QString& name)
{
    if(!m_aTexture.contains(name))
    {
        QSet<QString> texture;
        texture.insert(name);
        loadTexture(texture);
    }

    return m_aTexture.contains(name) ? m_aTexture[name] : textureDefault();
}

QOpenGLTexture* CTextureList::textureDefault()
{
    Q_ASSERT(m_aTexture.contains("default"));
    return m_aTexture[QString("default")];
}

void CTextureList::initResource()
{

    QImage img(":/default0.png", "PNG");
    QOpenGLTexture* texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::Repeat);
    texture->setSize(img.width(), img.height());
    texture->setFormat(QOpenGLTexture::TextureFormat::RGBA8_UNorm);
    texture->setData(img.mirrored());
    m_aTexture.insert(QString("default"), texture);

    initAuxTexture();

    if(true)
    {
        QSet<QString> aEmpty;
        loadTexture(aEmpty);
    }
}

struct STexSpecified
{
    STexSpecified(): startPos(76){}
    SMmpHeader header;
    QByteArray data;
    int startPos;
};

QOpenGLTexture* CTextureList::buildLandTex(QString& name, int& texCount)
{
    if(m_aTexture.contains(name))
    {
        texCount = m_aTexture[name]->width()/m_aTexture[name]->height();
        return m_aTexture[name];
    }

    QString tex;
    QVector<STexSpecified> aPart;

    //todo: remove code dublication. use the same way to load map texture and common texture {
    QVector<QFileInfo> fileInfo;
    auto pOpt = dynamic_cast<COptString*>(m_pSettings->opt(eOptSetResource, "texPath1"));
    if (!pOpt || pOpt->value().isEmpty())
    {
        Q_ASSERT("ahtung. texture resource not found" && false);
        QMessageBox::warning(m_pSettings, "Warning", "Choose path to textures.res");
        m_pSettings->onShow(eOptSetResource);
        //todo: wait until user choose resource file and continue
    }
    else
        fileInfo.append(pOpt->value());

    pOpt = dynamic_cast<COptString*>(m_pSettings->opt(eOptSetResource, "texPath2"));
    if(pOpt && !pOpt->value().isEmpty())
        fileInfo.append(pOpt->value());
    //<- todo}

    for(auto& file: fileInfo)
    {
        if(!aPart.isEmpty())
            break;

        ResFile res(file.filePath());
        QMap<QString, QByteArray> aFile = res.bufferOfFiles();
        for(int i(0); i<8; ++i)
        {
            tex = name + "00" + QString::number(i) + ".mmp";
            if(!aFile.contains(tex))
            {
                if(!aPart.empty())
                    break;

                continue;
            }

            STexSpecified part;
            part.data = aFile[tex];
            QDataStream stream(part.data);
            util::formatStream(stream);
            stream >> part.header;
            aPart.append(part);
        }
    }

    Q_ASSERT(!aPart.empty());
    //get min texture size
    int minTexSize(aPart.first().header.m_width);
    for(auto& part: aPart)
    {
        Q_ASSERT(part.header.m_width == part.header.m_height);
        if(part.header.m_width < minTexSize)
            minTexSize = part.header.m_width;
    }

    texCount = aPart.size();
    //choose mipMap
    int curMipMap(0), texSize(0), nByte(0);
    for(auto& part: aPart)
    {
        curMipMap = part.header.m_width/minTexSize/2;
        texSize = part.header.m_width;
        Q_ASSERT(curMipMap < part.header.m_mipcount);
        for(int i(0); i<curMipMap; ++i)
        {
            nByte = texSize*texSize/2;
            part.startPos += nByte;
            texSize /= 2;
        }

    }
    //combine texture data;
    QByteArray combineTextureData;
    const int rowSizeByte = 2*minTexSize; //8 byte == 1 block, minTexSize/4 - block count in row
    const int colSizeByte = rowSizeByte*minTexSize/4;
    for(int curByte(0); curByte<colSizeByte; curByte+=rowSizeByte) // todo: calc row bytes for 1 mipLevel
    {
        for(auto& part:aPart)
        {
            combineTextureData.append(part.data.mid(part.startPos, rowSizeByte));
            part.startPos += rowSizeByte;
        }
    }

    QOpenGLTexture* texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture->setMinificationFilter(QOpenGLTexture::NearestMipMapNearest);
    texture->setMagnificationFilter(QOpenGLTexture::NearestMipMapNearest);
    texture->setWrapMode(QOpenGLTexture::Repeat);
    texture->setFormat(QOpenGLTexture::TextureFormat::RGBA_DXT1); //todo: read texture format from file. EI can use both dxt3 and dxt1
    //texture->setMipLevels(4);
    texture->setSize(minTexSize*texCount, minTexSize);
    texture->allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
    const int size = minTexSize*minTexSize*texCount/2;
    texture->setCompressedData(0, 0, size, combineTextureData.data());
    m_aTexture[name] = texture;
    return texture;
}

CResourceManager::CResourceManager()
{

}

void CResourceManager::loadResources()
{
    QSet<QString> aEmpty;
    CTextureList::getInstance()->loadTexture(aEmpty);
}

bool isDIfferent(const QString &value)
{
    {
        return value == "<different>";
    }
}

QString valueDifferent()
{
    return "<different>";
}
