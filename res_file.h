#ifndef RES_FILE_H
#define RES_FILE_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QDateTime>
#include <QMap>
#include <QBuffer>
#include <QDataStream>

#pragma pack(push, 1)
struct SResFileHeader
{
    uint Signature;
    uint TableSize;
    uint TableOffset;
    uint NamesLenght;

    friend QDataStream& operator>> (QDataStream& st, SResFileHeader& head)
    {
        return st >> head.Signature >> head.TableSize >>
                     head.TableOffset >> head.NamesLenght;
    }

    friend QDataStream& operator<< (QDataStream& st, const SResFileHeader& head)
    {
        return st << head.Signature << head.TableSize <<
                     head.TableOffset << head.NamesLenght;
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SResHashTable
{
    uint NextIndex;
    uint DataSize;
    uint DataOffset;
    uint LastWriteTime;
    ushort NameLength;
    uint NameOffset;

    friend QDataStream& operator>> (QDataStream &st, SResHashTable &entry)
    {
        return st >> entry.NextIndex >> entry.DataSize >> entry.DataOffset >>
                     entry.LastWriteTime >> entry.NameLength >> entry.NameOffset;
    }

    friend QDataStream& operator<< (QDataStream &st, SResHashTable const &entry)
    {
        return st << entry.NextIndex << entry.DataSize << entry.DataOffset <<
                     entry.LastWriteTime << entry.NameLength << entry.NameOffset;
    }
};
#pragma pack(pop)

struct ResFileEntry
{
    QString FileName;
    int Position;
    int Size;
    QDateTime LastWriteTime;
};

///
/// \brief The ResFile class provides implementation of *.res files of the game
///
class ResFile
{
public:
    ResFile(QString path);
    ~ResFile();
    ResFile(const QByteArray& data);

//    void AddFile(QString& name, QDateTime time);

    QMap<QString, QByteArray>& bufferOfFiles() {return m_aFiles;}
//    QVector<ResFileEntry>& entries() {return m_aEntry;}
    void saveToFile(QString path);
    QByteArray generateResData();

private:
    //void completePreviousFile();
    //void WriteAlign();
    bool getFiles(QMap<QString, ResFileEntry>& aEntry, QDataStream& stream);
    void readFiles(QMap<QString, ResFileEntry>& aEntry, QDataStream& stream);
    void buildFileMap(QMap<QString, ResFileEntry>& aEntry,
                      QVector<SResHashTable>& fileTable, int offsetStream,
                      int streamLength, QString &nameBuf);
//    static QBuffer& ReadNamesByffer();
//    static void BuildResHashTable(
//            QVector<ResFileEntry>& entries, long headerPositon,
//            QVector<SResFileHashTableEntry> hashTable, QBuffer& namesBuffer);
//    static ushort AppendNamesBuffer(QBuffer& buffer, QString& name);

private:
    static const uint s_signature =  0x019CE23C;
    SResFileHeader m_header;
    //qint64 m_streamStartPos;
    //QDataStream* Stream;
    int m_bufLen;
    QVector<ResFileEntry> m_aEntry;
    QMap<QString, QByteArray> m_aFiles;



};

#endif // RES_FILE_H
