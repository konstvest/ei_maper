#include "resfile.h"

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
    file.close();

    Stream.setDevice(&file);
    Stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    Stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
}
