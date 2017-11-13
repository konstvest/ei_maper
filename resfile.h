#ifndef RESFILE_H
#define RESFILE_H

#include <QString>
#include <QDataStream>

struct ResFileEntry
{
public:
    QString   FileName;
    long     Position;
    int      Size;
    time_t LastWriteTime;
};

struct ResFileHeader
{
public:
    const uint ResFileSignature = 0x019CE23C;

    uint Signature;
    uint TableSize;
    uint TableOffset;
    uint NamesLength;
};

struct ResFileHashTableEntry
{
public:
    uint   NextIndex;
    uint   DataSize;
    uint   DataOffset;
    uint   LastWriteTime;
    ushort NameLength;
    uint   NameOffset;
};

class ResFile
{
public:
    ResFile(QByteArray stream);
    static QHash<QString, ResFileEntry> GetFiles(QDataStream stream);
    void AddFile(QString name, time_t time);
    QDataStream stream;
    void Free();

private:
    const long streamStartPosition;
    const QByteArray ByteArray;
    const QVector<ResFileEntry> entries;
    bool isFree;
    static QHash<QString, ResFileEntry> *BuildFileTableDictionary(
            QVector<ResFileHashTableEntry> *fileTable, long streamOffset, long streamLength, QString names);
    static ResFileHeader ReadResHeader(QDataStream stream);
    static QVector<ResFileHashTableEntry> *ReadResFileHashTable(QDataStream stream, uint size);
    static QByteArray ReadNamesBuffer(QDataStream stream, uint size);
    static void BuildResHashTable(
                QVector<ResFileEntry> *entries, long headerPosition,
                QVector<ResFileHashTableEntry> *hashTable, char *namesBuffer);
    static ushort AppendNamesBuffer(QVector<char> *buffer, QString name);
    void Complete();
    void CompletePreviousFile();
    void WriteAlign();

};

#endif // RESFILE_H
