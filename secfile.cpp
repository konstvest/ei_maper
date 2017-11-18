#include <QString>
//#include <qfile.h>
#include <secfile.h>
#include "ei_types.h"

bool SecFile::ReadSecFile(QString &path)
{
//    QFile mpFile(path);
//    mpFile.open(QIODevice::ReadOnly);
//    QDataStream st(&mpFile);
//    st.setByteOrder(QDataStream::ByteOrder::LittleEndian);
//    st.setFloatingPointPrecision(QDataStream::SinglePrecision);
    std::ifstream st;
    st.open(path.toStdString(), std::ios::binary);

    if(!st)
    {
        qDebug() << "Can't load mp file " << path;
        return false;
    }
    //st >> Header;


//    for(int i(0); i<VerticesCount; i++)
//    {
//        SecVertex sv;
//        st >> sv;
//        LandVertex.append(sv);
//    }

//    if(Header.Type == 3)
//    {
//        for(int i(0); i<VerticesCount; i++)
//        {
//            SecVertex sv;
//            st >> sv;
//            WaterVertex.append(sv);
//        }
//    }
//    else
//    {
//        for(int i(0); i<VerticesCount; i++)
//        {
//            SecVertex sv = {0, 0, 0, 0};
//            WaterVertex.append(sv);
//        }
//    }

//    for(int i(0); i<TilesCount; i++)
//    {
//        ushort lt;
//        st >> lt;
//        LandTiles.append(lt);
//    }

//    if(Header.Type == 3)
//    {
//        for(int i(0); i<TilesCount; i++)
//        {
//            ushort wt;
//            st >> wt;
//            WaterTiles.append(wt);
//        }

//        for(int i(0); i<TilesCount; i++)
//        {
//            ushort wa;
//            st >> wa;
//            WaterAllow.append(wa);
//        }
//    }
//    else
//    {
//        WaterTiles.append(0);
//        WaterAllow.append(65535);
//    }
}
