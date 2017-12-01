#ifndef RESFILE_H
#define RESFILE_H
#include <QString>
#include <QVector>
#include <QFile>
#include <QDateTime>
#include <QMap>
#include <QBuffer>
#include <QDataStream>

#pragma pack(push, 1)
struct ResFileHeader
{
    uint Signature;
    uint TableSize;
    uint TableOffset;
    uint NamesLenght;

    friend QDataStream& operator>> (QDataStream &st, ResFileHeader &head)
    {
        return st >> head.Signature >> head.TableSize >> head.TableOffset >>
                     head.NamesLenght;
    }

    friend QDataStream& operator<< (QDataStream &st, ResFileHeader const &head)
    {
        return st << head.Signature << head.TableSize << head.TableOffset <<
                     head.NamesLenght;
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ResFileHashTableEntry
{
    uint NextIndex;
    uint DataSize;
    uint DataOffset;
    uint LastWriteTime;
    ushort NameLength;
    uint NameOffset;

    friend QDataStream& operator>> (QDataStream &st, ResFileHashTableEntry &entry)
    {
        return st >> entry.NextIndex >> entry.DataSize >> entry.DataOffset >>
                     entry.LastWriteTime >> entry.NameLength >> entry.NameOffset;
    }

    friend QDataStream& operator<< (QDataStream &st, ResFileHashTableEntry const &entry)
    {
        return st << entry.NextIndex << entry.DataSize << entry.DataOffset <<
                     entry.LastWriteTime << entry.NameLength << entry.NameOffset;
    }
};
#pragma pack(pop)

struct ResFileEntry
{
    QString FileName;
    long Position;
    int Size;
    QDateTime LastWriteTime;
};

class ResFile
{
public:
    ResFile(QString& path);
    ~ResFile();
    ResFileHeader Header;

    void GetFiles(QDataStream& stream, QMap<QString, ResFileEntry>& entries);
    void AddFile(QString& name, QDateTime time);

    QMap<QString, QByteArray>& bufferOfFiles();
    QVector<ResFileEntry>& entries();
private:
    static const uint Signature =  0x019CE23C;
    bool IsRead = false;
    qint64 StreamStartPosition;
    //QDataStream* Stream;
    int BufferLength;
    QVector<ResFileEntry> Entries;
    QMap<QString, QByteArray> BufferOfFiles;

    void CompletePreviousFile();
    void WriteAlign();
    void GetBufferList(QMap<QString, ResFileEntry>& entries, QDataStream& stream);
    void BuildFileTableDictonary(
            QVector<ResFileHashTableEntry>& fileTable, long offsetStream,
            long streamLength, QString& names, QMap<QString, ResFileEntry>& entries);
    static QBuffer& ReadNamesByffer();
    static void BuildResHashTable(
            QVector<ResFileEntry>& entries, long headerPositon,
            QVector<ResFileHashTableEntry> hashTable, QBuffer& namesBuffer);
    static ushort AppendNamesBuffer(QBuffer& buffer, QString& name);

};

#endif // RESFILE_H
