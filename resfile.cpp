#include "resfile.h"
#include <QTextCodec>

ResFile::~ResFile()
{
    //this->Stream.device()->close();
}

void ResFile::BuildFileTableDictonary(
        QVector<ResFileHashTableEntry>& fileTable, long offsetStream,
        long streamLength, QString &names, QMap<QString, ResFileEntry>& entries)
{

    ResFileEntry entry;
    foreach (ResFileHashTableEntry file, fileTable)
    {
        if((long)(file.DataOffset + file.DataSize) > streamLength - offsetStream)
            throw "Invalid stream!";

        QString name = names.mid(file.NameOffset, file.NameLength);
        entry.FileName = name;
        entry.LastWriteTime = QDateTime::fromMSecsSinceEpoch(file.LastWriteTime);
        entry.Position = offsetStream + file.DataOffset;
        entry.Size = file.DataSize;

        entries.insert(name, entry);
    }

    //return result;
}

void ResFile::GetFiles(QDataStream &stream, QMap<QString, ResFileEntry>& entries)
{
    qint64 startPos = stream.device()->pos();
    
    stream >> Header;
    if(Header.Signature != Signature)
        throw "File isn't res-archive";
    stream.device()->seek(startPos + Header.TableOffset);
    QVector<ResFileHashTableEntry> fileTable;
    char bufferNames[Header.NamesLenght];
    
    ResFileHashTableEntry hashTable;
    for(uint i(0); i<Header.TableSize; i++)
    {
        stream >> hashTable;
        fileTable.append(hashTable);
    }

    int res = stream.readRawData((char*)&bufferNames, Header.NamesLenght);
    QTextCodec* codec = QTextCodec::codecForName("CP1251"); //m
    QString names = codec->toUnicode((char*)&bufferNames);

    BuildFileTableDictonary(fileTable, startPos, Buffer.length(), names, entries);
    
}

void ResFile::GetBufferList(QMap<QString, ResFileEntry> &entries, QDataStream &stream)
{
    foreach(auto entry, entries.values())
    {
        char arr[entry.Size];
        stream.writeRawData(arr, entry.Size);
        QByteArray ba(arr, entry.Size);
        bufferOfFiles.insert(entry.FileName, ba);
    }
}

ResFile::ResFile(QString &path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    if(file.error() != QFile::NoError)
    {
        if(file.error() != QFile::OpenError)
            file.close();
        throw "Error while open res-file";
    }
    Buffer = file.readAll();
    file.close();

    QDataStream tmpStream(Buffer);

    tmpStream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    tmpStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    StreamStartPosition = tmpStream.device()->pos();
    QMap<QString, ResFileEntry> resEntries;
    GetFiles(tmpStream, resEntries);
    GetBufferList(resEntries, tmpStream);
}
