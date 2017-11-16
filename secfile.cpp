#include <QString>
#include <qfile.h>
#include <secfile.h>

void SecFile::ReadSecFile(QString &path)
{
    QFile mpFile(path);
    mpFile.open(QIODevice::ReadOnly);
    QDataStream st(&mpFile);
    st.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    st.setFloatingPointPrecision(QDataStream::SinglePrecision);

    st >> Header;

    for(int i(0); i<VerticesCount; i++)
    {
        SecVertex sv;
        st >> sv;
        LandVertex.append(sv);
    }

    if(Header.Type == 3)
    {
        for(int i(0); i<VerticesCount; i++)
        {
            SecVertex sv;
            st >> sv;
            WaterVertex.append(sv);
        }
    }
    else
    {
        for(int i(0); i<VerticesCount; i++)
        {
            SecVertex sv = {0, 0, 0, 0};
            WaterVertex.append(sv);
        }
    }
}
