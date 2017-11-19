#ifndef EI_UTILS_H
#define EI_UTILS_H
#include "ei_types.h"
#include <QString>
#include <QMap>

class EI_Utils
{
public:
    enum eSignatures
    {
        fig8 = 1,
        mp = 2,
        sec = 3
    };
    static bool checkSignature(std::ifstream& file, eSignatures sigKey);
    static struct Messages
    {
        const QString SecOpenError = "Sec-file must be initialize, use ReadFromFile function for initialize";
        const QString MpOpenError = "Mp-file must be initialize, use ReadFromFile function for initialize";
        const QString CantLoadFile = "Can't load file from: ";
        Messages()
        {
        }
    }messages;
private:
    static QMap<int, uint> SignaturesCollection;
    EI_Utils()
    {

    }
};
#endif // EI_UTILS_H
