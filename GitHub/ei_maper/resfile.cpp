#include "resfile.h"
#include <QString>
#include <QDataStream>
#include <QByteArray>
#include <QFile>

QDataStream &operator >>(QDataStream &ds, ResFileHeader &rfh)
{
    return out >> rfh.Signature >> rfh.TableSize >> rfh.TableOffset >> rfh.NamesLength;

}

QDataStream &operator <<(QDataStream &ds, ResFileHeader &rfh)
{
    return in << rfh.Signature << rfh.TableSize << rfh.TableOffset << rfh.NamesLength;
}

ResFile::ResFile(QByteArray byteArray)
{
    if(stream == NULL)
        throw "Argument \"stream\" is NULL";
    this->isFree = false;
    this->stream = QDataStream(&byteArray, QIODevice::OpenModeFlag);
    this->streamStartPosition = stream.device()->pos();

    ResFileHeader header =
    {
        Signature = ResFileHeader.ResFileSignature,
        TableSize = 0,
        TableOffset = 0,
        NamesLength = 0
    };

    stream << header;
}

QHash<QString, ResFileEntry> ResFile::GetFiles(QDataStream stream)
{
    if(stream == NULL)
        throw "Argument \"stream\" is NULL";

    long streamStartPosition = stream.device()->pos();
    ResFileHeader header = ReadResHeader(stream);

    if (header.Signature != ResFileHeader.ResFileSignature)
        throw "Invalid header for resfile";
    stream.device()->seek(streamStartPosition + header.TableOffset);
    QVector<ResFileHashTableEntry> *fileTable = ReadResFileHashTable(stream, header.TableSize);
    QByteArray names = ReadNamesBuffer(stream, header.NamesLength);

    QHash<QString, ResFileEntry> result = BuildFileTableDictionary(fileTable, streamStartPosition, stream.device()->bytesAvailable(), names.toShort());
    return result;
}
