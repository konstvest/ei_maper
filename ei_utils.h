#ifndef EI_UTILS_H
#define EI_UTILS_H
#include "ei_types.h"
#include <QMap>

class EI_Utils
{
public:
    enum Signatures
    {
        fig8 = 1,
        mp = 2,
        sec = 3
    };
    static bool checkSignature(std::ifstream& file, Signatures sigKey);
private:
    static QMap<int, std::string> SignaturesCollect;
    EI_Utils()
    {

    }
};
#endif // EI_UTILS_H
