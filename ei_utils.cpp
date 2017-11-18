#include "ei_utils.h"

static QMap<int, std::string> SetCollection()
{
    QMap<int, std::string> map;
    map.insert(1, "FIG8");
    map.insert(2, "");
    map.insert(3, "");

    return map;
}

QMap<int, std::string> EI_Utils::SignaturesCollect = SetCollection();

bool EI_Utils::checkSignature(std::ifstream& file, Signatures sigKey)
{

    std::string signature = SignaturesCollect.value(sigKey, 0);
    char buf[signature.length()];
    file.read(buf, sizeof(buf));
    std::string readString(buf);
    if (readString != signature)
    {
        qDebug() << "incorrect signature";
        file.close();
        return false;
    }
    return true;
}

