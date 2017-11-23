#include "ei_utils.h"

//static QMap<int, uint> SetCollection()
//{
//    QMap<int, uint> map;
//    map.insert(1, 0x38474946); //fig8
//    map.insert(2, 0xce4af672); //mp
//    map.insert(3, 0xcf4bf774); //sec

//    return map;
//}

//QMap<int, uint> EI_Utils::SignaturesCollection = SetCollection();

EI_Utils::Messages EI_Utils::messages;

//bool EI_Utils::checkSignature(std::ifstream& file, eSignatures sigKey)
//{
//    uint readsign;
//    uint signature = SignaturesCollection[sigKey];
//    file.read((char*)&readsign, sizeof(signature));
//    if (readsign != signature)
//    {
//        qDebug() << "incorrect signature";
//        file.close();
//        return false;
//    }
//    return true;
//}

