#include "QDebug"
#include <QTextCodec>
#include "res_file.h"
#include "utils.h"

void ResFile::buildFileMap(
        QMap<QString, ResFileEntry>& aEntry,
        QVector<SResHashTable>& fileTable, int offsetStream,
        int streamLength, QString &nameBuf)
{

    ResFileEntry entry;
    for (auto& file: fileTable)
    {
        if(int(file.DataOffset + file.DataSize) > streamLength - offsetStream)
            throw new std::runtime_error("Invalid stream!");

        entry.FileName = nameBuf.mid(int(file.NameOffset), file.NameLength);
        entry.LastWriteTime = QDateTime::fromMSecsSinceEpoch(file.LastWriteTime);
        entry.Position = offsetStream + int(file.DataOffset);
        entry.Size = int(file.DataSize);
        aEntry.insert(entry.FileName, entry);
    }

}

bool ResFile::getFiles(QMap<QString, ResFileEntry>& aEntry, QDataStream& stream)
{
    qint64 startPos = stream.device()->pos();

    stream >> m_header;
    if (m_header.Signature != s_signature)
        return false;

    stream.device()->seek(startPos + m_header.TableOffset);
    QVector<SResHashTable> aFile;
    QVector<char> aName;
    aName.resize(int(m_header.NamesLenght));

    SResHashTable hashTable;
    for (uint i(0); i<m_header.TableSize; ++i)
    {
        stream >> hashTable;
        aFile.append(hashTable);
    }

    stream.readRawData(aName.data(), int(m_header.NamesLenght));
    QTextCodec* codec = QTextCodec::codecForName("CP1251");
    QString name = codec->toUnicode(aName.data());
    buildFileMap(aEntry, aFile, int(startPos), m_bufLen, name);

    return true;
}

// in: aEntry, stream
void ResFile::readFiles(QMap<QString, ResFileEntry>& aEntry, QDataStream& stream)
{
    for (auto& entry: aEntry.values())
    {
        QByteArray ba(entry.Size, 0);
        stream.device()->seek(entry.Position);
        stream.readRawData(ba.data(), entry.Size);
        m_aFiles.insert(entry.FileName, ba);
    }
}

uint getEIStringHash32(QString value, uint hashTableSize = 0)
{
    if (value.isEmpty())
    {
        ei::log(eLogFatal, "Hash for empty string is incorrect");
        return -1;
    }

    uint hash = 0;
    QTextCodec* pCodec = QTextCodec::codecForName("CP1251");
    QByteArray filenameAsByte = pCodec->fromUnicode(value.toLower());
    for(auto& symb: filenameAsByte)
    {
        hash += symb;
    }

    return hashTableSize == 0
        ? hash
        : hash % hashTableSize;
}



void buildResHashTable(QMap<QString, QByteArray> entries, uint& tableOffset,
                       QVector<SResHashTable>& outHashTable, QVector<uint>& outAlignOffset, QByteArray& outName)
{
    uint hashTableSize = (uint)entries.size();
    outName.clear();
    outHashTable.resize(hashTableSize);
    for (int i = 0; i < outHashTable.size(); i++)
    {
        outHashTable[i].NextIndex = -1;
        outHashTable[i].DataOffset = 0;
    }
    QDateTime now(QDateTime::currentDateTime());
    uint unixTime = now.toTime_t();

    uint lastFreeIndex = (uint)outHashTable.size() - 1;
    for (auto& entry: entries.toStdMap())
    {
        uint index = getEIStringHash32(entry.first, hashTableSize);
        if (outHashTable[index].DataOffset != 0)
        {
            while (outHashTable[index].NextIndex != uint(-1))
                index = outHashTable[index].NextIndex;

            while (outHashTable[lastFreeIndex].DataOffset != 0)
                lastFreeIndex--;

            outHashTable[index].NextIndex = lastFreeIndex;
            index = lastFreeIndex;
            lastFreeIndex--;
        }

        outHashTable[index].LastWriteTime = unixTime;
        outHashTable[index].DataOffset    = (uint)tableOffset; tableOffset += entry.second.size();
        uint alignOffset = 16-tableOffset%16;
        outAlignOffset.append(alignOffset);
        tableOffset += alignOffset;

        outHashTable[index].DataSize      = (uint)entry.second.size();
        outHashTable[index].NameOffset    = (uint)outName.size();
        outHashTable[index].NameLength    = (uint)entry.first.length(); outName.append(entry.first);
        outHashTable[index].NextIndex = -1;
    }
}

QByteArray ResFile::generateResData()
{
    if(m_aFiles.empty())
    {
        ei::log(eLogWarning, "Can not write res-file with empty data");
    }
    SResFileHeader header{s_signature, uint(m_aFiles.size()), sizeof (SResFileHeader), 0};

    QVector<SResHashTable> aHashTable;
    QVector<uint> aAlignOffset;
    QByteArray nameData;
    buildResHashTable(m_aFiles, header.TableOffset, aHashTable, aAlignOffset, nameData);
    header.NamesLenght = nameData.size();
    QByteArray resData;
    QDataStream resStream(&resData, QIODevice::WriteOnly);
    util::formatStream(resStream);
    //write header
    resStream << header;
    //write file data
    char zeroByte = 0;

    int nOffset = 0;
    for(auto& file: m_aFiles)
    {
        resStream.writeRawData(file, file.size());
        for(uint i(0); i<aAlignOffset[nOffset]; ++i)
        {
            resStream.writeRawData(&zeroByte, 1);
        }
        ++nOffset;
    }
    //write hash table
    for(auto& hashData: aHashTable)
        resStream << hashData;
    //write name data
    resStream.writeRawData(nameData, nameData.size());
    return resData;
}

ResFile::ResFile(QString path)
{
    m_aFiles.clear();
    m_aEntry.clear();

    QFile file(path);
    if (!file.exists())
    {
        qDebug() << file.fileName() << " not exists";
        return;
    }

    try
    {
        file.open(QIODevice::ReadOnly);
        if (file.error() != QFile::NoError)
        {
            qDebug() << file.fileName() << " Error while open res-file";
            return;
        }

        QByteArray buffer = file.readAll();
        m_bufLen = buffer.length();
        file.close();

        QDataStream stream(buffer);
        util::formatStream(stream);
        QMap<QString, ResFileEntry> resEntries;
        if (!getFiles(resEntries, stream))
        {
            qDebug() << "Incorrect file signature";
            buffer.clear();
            file.close();
            return;
        }
        readFiles(resEntries, stream);
        buffer.clear();
    }
    catch (std::exception ex)
    {
        qDebug() << ex.what();
    }
}

ResFile::ResFile(const QByteArray& data)
{
    m_bufLen = data.length();
    QDataStream stream(data);
    util::formatStream(stream);
    QMap<QString, ResFileEntry> resEntries;
    if (!getFiles(resEntries, stream))
    {
        qDebug() << "Incorrect file signature";
        return;
    }
    readFiles(resEntries, stream);
}

void ResFile::saveToFile(QString path)
{
    QFile file(path);
    if (file.exists())
    {
        qDebug() << file.fileName() << " already exists";
        return;
    }

    try
    {
        file.open(QIODevice::WriteOnly);
        if (file.error() != QFile::NoError)
        {
            qDebug() << file.fileName() << " Error while writing res-file";
            return;
        }
        QByteArray resData = generateResData();
        file.write(resData);
        file.close();
    }
    catch (std::exception ex)
    {
        qDebug() << ex.what();
    }
}

ResFile::~ResFile()
{
    for (auto& buf: m_aFiles.values())
    {
        buf.clear();
    }
    m_aFiles.clear();
    m_aEntry.clear();
}
