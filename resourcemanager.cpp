#include "resourcemanager.h"
#include <QDebug>
#include <QMessageBox>
#include <QtMath>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>

#include "res_file.h"
#include "utils.h"
#include "view.h"
#include "types.h"
#include "settings.h"
#include "log.h"

CObjectList* CObjectList::m_pObjectContainer = nullptr;
CTextureList* CTextureList::m_pTextureContainer = nullptr;

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

    CResFile res(auxFile.filePath());
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
    CResFile model(aFile[assemblyRoot]);
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
    CResFile position(aFile[bonFile]);
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
    auto pOpt = dynamic_cast<COptStringList*>(m_pSettings->opt(eOptSetResource, "figPaths"));
    if (!pOpt || pOpt->value().isEmpty())
    {
        QMessageBox::warning(m_pSettings, "Warning","Choose path to figures.res");
        m_pSettings->onShow(eOptSetResource);
        //todo: wait until user choose resource file and continue
        return;
    }

    uint n(0);
    QString figName;
    for(auto& file: pOpt->value())
    {
        CResFile res(file);
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
    texture->setWrapMode(QOpenGLTexture::Repeat);
    switch (header.m_format)
    {
    case ETextureFormat::eMMP_DXT1:
    {
        texture->setFormat(QOpenGLTexture::TextureFormat::RGBA_DXT1);
        texture->setSize(int(header.m_width), int(header.m_height));
        texture->allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
        const int size = int(header.m_width * header.m_height)/2;
        texture->setCompressedData(0, 0, size, data.data() + header.size());
        break;
    }
    case ETextureFormat::eMMP_DXT3:
    {
        texture->setFormat(QOpenGLTexture::TextureFormat::RGBA_DXT3);
        texture->setSize(int(header.m_width), int(header.m_height));
        texture->allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
        int offset = header.size();
        for(int i(0); i<header.m_mipcount; ++i)
        {
            const int size = header.m_width / qPow(2,i) * header.m_height / qPow(2,i);
            texture->setCompressedData(i, 0, size, data.data() + offset);
            offset += size;
        }
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
    m_aTexture.insert(name.toLower(), texture);
}

void CTextureList::initAuxTexture()
{
    //read textures for aux objects.
    //TODO: can be conflict with user tex name. load aux textures in separate map
    auto auxFile = QFileInfo(":/auxData.res");

    CResFile res(auxFile.filePath());
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
    auto pOpt = dynamic_cast<COptStringList*>(m_pSettings->opt(eOptSetResource, "texPaths"));
    if (!pOpt || pOpt->value().isEmpty())
    {
        QMessageBox::warning(m_pSettings, "Warning", "Choose path to textures.res");
        m_pSettings->onShow(eOptSetResource);
        //todo: wait until user choose resource file and continue
        return;
    }

    QString texName;

    uint n(0);
    for(auto& file: pOpt->value())
    {
        CResFile res(file);
        QMap<QString, QByteArray> aFile = res.bufferOfFiles();
        if (aName.isEmpty())
        {

            //load all textures exclude "special"
            QRegExp rx("((material|spell|modifier|prototype|qitem|quitem|litem|skill)\\d{2,4})|(^\\S+\\d{3})|(_\\d{2}\\.\\d)|(face\\S*\\d+\\S*)|((sm_)?cursor_\\S+)|(zone\\S+(questm?|map))|(un(mo|un)\\S+w[1-3])"); //TODO: delete this when loading textures will be faster...
            rx.setCaseSensitivity(Qt::CaseInsensitive);
            for (auto& packedTex : aFile.toStdMap())
            {
                texName = packedTex.first.split(".")[0];
                texName = texName.toLower();
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
                texName = name.toLower();
                //if(!name.contains(".mmp")) continue;
                if(m_aTexture.contains(texName)) continue;
                if(!aFile.contains(name + ".mmp")) continue;

                parse(aFile[name + ".mmp"], name);
            }
    }
    std::sort(m_arrCellComboBox.begin(), m_arrCellComboBox.end());
}

QOpenGLTexture* CTextureList::texture(const QString& name)
{
    QString texName(name.toLower());
    if(!m_aTexture.contains(texName))
    {
        QSet<QString> texture;
        texture.insert(texName);
        loadTexture(texture);
    }

    return m_aTexture.contains(texName) ? m_aTexture[texName] : textureDefault();
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

QImage convert_DXT(QDataStream& stream, int width, int height, bool DXT3 = false)
{
    QImage image(width, height, QImage::Format_RGBA8888);
    uint16_t color[4][4] = {};

    for (int i = 0; i < height / 4; i++) {
        for (int j = 0; j < width / 4; j++) {
            if (DXT3) {
                for (int x = 0; x < 4; x++) {
                    uint16_t row;
                    stream >> row;
                    for (int y = 0; y < 4; y++) {
                        int alpha = (row & 15) * 17;
                        row >>= 4;
                        image.setPixel(j * 4 + y, i * 4 + x, QColor(0, 0, 0, alpha).rgba());
                    }
                }
            }

            uint16_t gen_c1, gen_c2;
            stream >> gen_c1;
            stream >> gen_c2;

            color[0][0] = extractColor(gen_c1, 63488, 11);
            color[0][1] = extractColor(gen_c1, 2016, 5);
            color[0][2] = extractColor(gen_c1, 31, 0);

            color[1][0] = extractColor(gen_c2, 63488, 11);
            color[1][1] = extractColor(gen_c2, 2016, 5);
            color[1][2] = extractColor(gen_c2, 31, 0);

            if (gen_c1 > gen_c2 || DXT3) {
                for (int k = 0; k < 3; k++) {
                    color[2][k] = (2 * color[0][k] + color[1][k]) / 3;
                    color[3][k] = (color[0][k] + 2 * color[1][k]) / 3;
                }
            } else {
                for (int k = 0; k < 3; k++) {
                    color[2][k] = (color[0][k] + color[1][k]) / 2;
                    color[3][k] = 0;
                }
            }

            for (int x = 0; x < 4; x++) {
                uint8_t row;
                stream >> row;
                for (int y = 0; y < 4; y++) {
                    int idx = row & 3;
                    QColor pixelColor(color[idx][0], color[idx][1], color[idx][2], (DXT3 ? image.pixelColor(j * 4 + y, i * 4 + x).alpha() : 255));
                    image.setPixel(j * 4 + y, i * 4 + x, pixelColor.rgba());
                    row >>= 2;
                }
            }
        }
    }

    return image;
}

int CTextureList::extractMmpToDxt1(QVector<QImage>& outArrImage, const QStringList& inArrTextureName)
{
    outArrImage.clear();
    outArrImage.resize(inArrTextureName.size());
    QStringList arrPath;

    auto pOpt = dynamic_cast<COptStringList*>(m_pSettings->opt(eOptSetResource, "texPaths"));
    if(pOpt && !(pOpt->value().isEmpty()))
        arrPath = QStringList::fromVector(pOpt->value());

    int nCount(0);
    for(auto& path: arrPath)
    {
        if(nCount == inArrTextureName.size()) //all textures already found
            break;
        CResFile res(path);
        QMap<QString, QByteArray> aFile = res.bufferOfFiles();

        for(const auto& texName: inArrTextureName)
        {
            if(!aFile.contains(texName))
                continue;

            QDataStream stream(aFile[texName]);
            util::formatStream(stream);

            SMmpHeader header;
            stream >> header;
            if(header.m_signature != 0x00504D4D)
            {
                Q_ASSERT("incorrect texture signature" && false);
                return -1;
            }
            if(header.m_format != ETextureFormat::eMMP_DXT1)
            {
                ei::log(eLogWarning, "texture has a different format than dxt1 or dxt3");
                continue;
            }

            stream.device()->seek(header.size());
            QImage& img = outArrImage[inArrTextureName.indexOf(texName)];
            img =  convert_DXT(stream, header.m_width, header.m_height);
            img = img.mirrored(false, true);
            ++nCount;
        }
    }
    return nCount == inArrTextureName.size();
}

int CTextureList::extractMmpToDxt1(QImage& outImage, const QString textureName)
{
    QVector<QImage> arrImage;
    QStringList arrTexName;
    arrTexName.append(textureName);
    int res = extractMmpToDxt1(arrImage, arrTexName);
    if(res == 0)
        outImage = arrImage.front();
    return res;
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
    auto pOpt = dynamic_cast<COptStringList*>(m_pSettings->opt(eOptSetResource, "texPaths"));
    if (!pOpt || pOpt->value().isEmpty())
    {
        Q_ASSERT("ahtung. texture resource not found" && false);
        QMessageBox::warning(m_pSettings, "Warning", "Choose path to textures.res");
        m_pSettings->onShow(eOptSetResource);
        //todo: wait until user choose resource file and continue
    }
    //<- todo}

    for(auto& file: pOpt->value())
    {
        if(!aPart.isEmpty())
            break;

        CResFile res(file);
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

    if(aPart.empty())
    {
        ei::log(ELogMessageType::eLogWarning, "cannot find texture for zone, loading default");
        STexSpecified part;
        auto auxFile = QFileInfo(":/auxData.res");
        CResFile res(auxFile.filePath());
        QMap<QString, QByteArray> aFile = res.bufferOfFiles();
        part.data = aFile["default_zone.mmp"];
        QDataStream stream(part.data);
        util::formatStream(stream);
        stream >> part.header;
        for(int i(0); i<8; ++i)
        {
            aPart.append(part);
        }
    }
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

CResourceStringList* CResourceStringList::m_pResourceStringContainer = nullptr;

CResourceStringList* CResourceStringList::getInstance()
{
    if(nullptr == m_pResourceStringContainer)
        m_pResourceStringContainer = new CResourceStringList();
    return m_pResourceStringContainer;
}

CResourceStringList::CResourceStringList()
{
    initResourceString();
}

bool CResourceStringList::getPropList(QMap<uint, QString>& map, const EObjParam propType)
{
    map.clear();
    if(!m_propValueName.contains(propType))
        return false;

    map = m_propValueName[propType];
    return true;
}

//todo: move string outside hard code to localization file
void CResourceStringList::initResourceString()
{
    QMap<uint, QString> map;
    map[0] = "Not active";
    map[1] = "Hand";
    map[2] = "Picklock (unused)";
    map[4] = "Crowbar (unused)";
    map[5] = "Hand or Crowbar";
    map[8] = "Key";
    map[9] = "Hand or Key";
    m_propValueName[eObjParam_LEVER_SCIENCE_STATS_Type_Open] = map;

    map.clear();
    {
        for(int i(0); i < 32; ++i)
            map[i] = "Player-" + QString::number(i);
    }
    m_propValueName[eObjParam_PLAYER] = map;

    map.clear();
    map[0] = "False";
    map[1] = "True";
    m_propValueName[eObjParam_IS_SHADOW] = map;
    m_propValueName[eObjParam_LEVER_IS_CYCLED] = map;
    m_propValueName[eObjParam_LEVER_IS_DOOR] = map;
    m_propValueName[eObjParam_USE_IN_SCRIPT] = map;
    m_propValueName[eObjParam_LEVER_RECALC_GRAPH] = map;
    m_propValueName[eObjParam_UNIT_NEED_IMPORT] = map;
    m_propValueName[eObjParam_SOUND_AMBIENT] = map;
    m_propValueName[eObjParam_SOUND_IS_MUSIC] = map;
    m_propValueName[eObjParam_LIGHT_SHADOW] = map;
    m_propValueName[eObjParam_TRAP_CAST_ONCE] = map;
    m_propValueName[eObjParam_ALWAYS_ACTIVE] = map;

    map.clear();
    map[50] = "Human";
    map[51] = "Animal";
    map[52] = "Monstr";
    m_propValueName[eObjParam_TYPE] = map;

    map.clear();

    map[8192] = "Fireball";
    map[8193] = "Campfire";
    map[8194] = "Fireblast";
    map[8195] = "Fire";
    map[8196] = "Smoke";
    map[8197] = "Vulcansmoke";
    map[8198] = "Healing";
    map[8199] = "Poisonfog";
    map[8200] = "Aggressionfog";
    map[8201] = "Geyser";
    map[8202] = "Tornado";
    map[8203] = "Casting";
    map[8204] = "Nuke";
    map[8205] = "Bansheecasting";
    map[8206] = "Mushroom";
    map[8207] = "Blood1";
    map[8208] = "Firewall";
    map[8209] = "Firearrow";
    map[8210] = "Acidray";
    map[8211] = "Bluegas";
    map[8212] = "Link";
    map[8213] = "Sphereacid";
    map[8214] = "Sphereelectricity";
    map[8215] = "Spherefire";
    map[8216] = "Clayring";
    map[8217] = "Teleport";
    map[8218] = "Antimagic";
    map[8219] = "Modifier1";
    map[8220] = "Modifier2";
    map[8221] = "Modifier3";
    map[8222] = "Modifier4";
    map[8223] = "Modifier5";
    map[8224] = "Modifier6";
    map[8225] = "Modifier7";
    map[8226] = "Modifier8";
    map[8227] = "Modifier9";
    map[8228] = "Modifier10";
    map[8229] = "Modifier11";
    map[8230] = "Modifier12";
    map[8231] = "Castingfire";
    map[8232] = "Castingelectricity";
    map[8233] = "Castingacid";
    map[8234] = "Castingdivination";
    map[8235] = "Castingillusion";
    map[8236] = "Castingdomination";
    map[8237] = "Castingenchantment";
    map[8238] = "Castinghealing";
    map[8239] = "Castingfailed";
    map[8240] = "Lightningblast";
    map[8241] = "Blood2";
    map[8242] = "Blood3";
    map[8243] = "Blood4";
    map[8244] = "A4bloodred";
    map[8245] = "A4bloodgreen";
    map[8246] = "A4bloodblue";
    map[8247] = "A4bloodblack";
    map[8248] = "Zoneexit";
    map[8249] = "Path";
    map[8250] = "Pathdestination";
    map[8251] = "Pathfailed";
    map[8252] = "Moshka";
    map[8253] = "Portalstar";
    map[8254] = "Portal";
    map[8255] = "Cylinder1";
    map[8256] = "Cylinder2";
    map[8257] = "Firestar";
    map[8258] = "Acidstar";
    map[8259] = "Sparks";
    map[8260] = "Visionstar1";
    map[8261] = "Visionstar2";
    map[8262] = "Visionstar3";
    map[8263] = "Regeneration";
    map[8264] = "Silence";
    map[8265] = "Feeblemind";
    map[8266] = "Feetcloud1";
    map[8267] = "Feetcloud2";
    map[8268] = "Visionstar4";
    map[8269] = "Ballofstars";
    map[8270] = "Rickarrow";
    map[8271] = "Cursestars";
    map[8272] = "Curseholder";
    map[8273] = "Starttrans";
    map[8274] = "Transform";
    m_propValueName[eObjParam_PARTICL_TYPE] = map;

    map.clear();
    map[0] = "Idle";
    map[1] = "Guard radius";
    map[2] = "Path";
    map[3] = "Place";
    map[4] = "Briffing";
    map[5] = "Guard Alaram";
    m_propValueName[eObjParam_LOGIC_BEHAVIOUR] = map;

    map.clear();
    map[0] = "Attack";
    map[1] = "Revenge";
    map[2] = "Fear";
    map[3] = "Fear player";
    m_propValueName[eObjParam_AGRESSION_MODE] = map;

    map.clear();
    map[0] = "HP";
    map[1] = "Max HP";
    map[2] = "MP";
    map[3] = "Max MP";
    map[4] = "Tuning Move";
    map[5] = "Actions";
    map[6] = "Speed Run";
    map[7] = "Speed Walk";
    map[8] = "Speed Crouch";
    map[9] = "Speed Crawl";
    map[10] = "Vision Arc";
    map[11] = "Skills Peripherial";
    map[12] = "Peripherial Arc";
    map[13] = "Attack Distance";
    map[14] = "AI Class Stay";
    map[15] = "AI Class Lay";
    map[16] = "Reserved";
    map[17] = "Range";
    map[18] = "Attack";
    map[19] = "Defence";
    map[20] = "Weight";
    map[21] = "Damage Min";
    map[22] = "Damage Range";
    map[23] = "Armor Impalling";
    map[24] = "Armor Slashing";
    map[25] = "Armor Crushing";
    map[26] = "Armor Thermal";
    map[27] = "Armor Chemical";
    map[28] = "Armor Electrical";
    map[29] = "Armor General";
    map[30] = "Absorption";
    map[31] = "Sight";
    map[32] = "Night Sight";
    map[33] = "Sense Life";
    map[34] = "Sense Hear";
    map[35] = "Sense Smell";
    map[36] = "Sense Tracking";
    map[37] = "pSight";
    map[38] = "pNight Sight";
    map[39] = "pSense Life";
    map[40] = "pSense Hear";
    map[41] = "pSense Smell";
    map[42] = "pSense Tracking";
    map[43] = "Manual Skill Science";
    map[44] = "Manual Skill Stealing";
    map[45] = "Manual Skill Tame";
    map[46] = "Magical Skill 1";
    map[47] = "Magical Skill 2";
    map[48] = "Magical Skill 3";
    map[49] = "Reserved";
    map[50] = "Reserved";
    m_propValueName[eObjParam_UNIT_STATS] = map;

    m_tileType[ETileType::eGrass]     = "Grass";
    m_tileType[ETileType::eGround]    = "Ground";
    m_tileType[ETileType::eStone]     = "Stone";
    m_tileType[ETileType::eSand]      = "Sand";
    m_tileType[ETileType::eRock]      = "Rock";
    m_tileType[ETileType::eField]     = "Field";
    m_tileType[ETileType::eWater]     = "Water";
    m_tileType[ETileType::eRoad]      = "Road";
    m_tileType[ETileType::eUndefined] = "Undefined";
    m_tileType[ETileType::eSnow]      = "Snow";
    m_tileType[ETileType::eIce]       = "Ice";
    m_tileType[ETileType::eDrygrass]  = "Drygrass";
    m_tileType[ETileType::eSnowballs] = "Snowballs";
    m_tileType[ETileType::eLava]      = "Lava";
    m_tileType[ETileType::eSwamp]     = "Swamp";
    m_tileType[ETileType::eHighrock]  = "Highrock";

    m_materialType[ETerrainType::eTerrainNoWater] = "Liquid (Water) disabled";
    m_materialType[ETerrainType::eTerrainWater] = "Liquid (Water)";

}

CNvttManager::CNvttManager(QString appDir):
    m_bInit(false)
{
    m_nvcompress = appDir + QDir::separator() + "nvtt" + QDir::separator() + "nvcompress.exe";
    if(!QFile::exists(m_nvcompress))
    {
        ei::log(eLogFatal, "Cannot find nvcompress.exe for dxt converting");
    }
    m_bInit = true;
}

int CNvttManager::dxtToBmp(QString pathToDxt, QString pathToBmp)
{
    if(!m_bInit)
    {
        ei::log(eLogWarning, "Cannot convert dxt data. CNvttManager not initialized. Check nvcompress.exe");
        return -1;
    }
    // nvcompress params
    QStringList arguments;
    arguments << "-d" << pathToDxt << pathToBmp;

    QProcess process;
    //process.start(m_nvcompress, arguments);
    QString cmd = QString("\"%1\" \"%3\" \"%4\"").arg(m_nvcompress, pathToDxt, pathToBmp);
    process.start(cmd);

    // check if process running
    if (!process.waitForStarted()) {
        qDebug() << "Cannot initialise process!";
        return -1;
    }

    if (!process.waitForFinished()) {
        qDebug() << "process suspended";
        return -1;
    }

    QString output = process.readAllStandardOutput();
    ei::log(eLogDebug, "NVTT conversion: " + output);

    QString errorOutput = process.readAllStandardError();
    if (!errorOutput.isEmpty()) {
        qDebug() << "conversion error: " << errorOutput;
    }
    return 0;
}
