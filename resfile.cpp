#include "resfile.h"
#include <QTextCodec>

ResFile::~ResFile()
{
    //this->Stream.device()->close();
}

QMap<QString, ResFileEntry>& ResFile::BuildFileTableDictonary(
        QVector<ResFileHashTableEntry>& fileTable, long offsetStream,
        long streamLength, QString &names)
{
    QMap<QString, ResFileEntry> result;

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

        result.insert(name, entry);
    }

    return result;
}

QMap<QString, ResFileEntry>& ResFile::GetFiles(QDataStream &stream)
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

    return BuildFileTableDictonary(fileTable, startPos, Buffer.length(), names);
    
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
    QMap<QString, ResFileEntry> resEntry = GetFiles(tmpStream);
}
