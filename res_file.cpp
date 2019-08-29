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

ResFile::~ResFile()
{
    for (auto& buf: m_aFiles.values())
    {
        buf.clear();
    }
    m_aFiles.clear();
    m_aEntry.clear();
}
