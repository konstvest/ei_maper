#ifndef RES_FILE_H
#define RES_FILE_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QDateTime>
#include <QMap>
#include <QBuffer>
#include <QDataStream>

struct SResFileEntry
{
    QString FileName;
    int Position;
    int Size;
    QDateTime LastWriteTime;
};

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

///
/// \brief The CResFile class provides implementation of *.res files of the game
///
class CResFile
{
public:
    CResFile(QString path);
    ~CResFile();
    CResFile(const QByteArray& data);

    QMap<QString, QByteArray>& bufferOfFiles() {return m_aFiles;}
    void saveToFile(QString path);
    QByteArray generateResData();

private:
    bool getFiles(QMap<QString, SResFileEntry>& aEntry, QDataStream& stream);
    void readFiles(QMap<QString, SResFileEntry>& aEntry, QDataStream& stream);
    void buildFileMap(QMap<QString, SResFileEntry>& aEntry,
                      QVector<SResHashTable>& fileTable, int offsetStream,
                      int streamLength, QString &nameBuf);

private:
    static const uint s_signature =  0x019CE23C;
    SResFileHeader m_header;
    int m_bufLen;
    QVector<SResFileEntry> m_aEntry;
    QMap<QString, QByteArray> m_aFiles;



};

#endif // RES_FILE_H
