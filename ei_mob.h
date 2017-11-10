#ifndef EI_MOB_H
#define EI_MOB_H

#include "ei_types.h"

namespace ei {

class CMob{
public:
    CMob();
    bool loadFromFile(QString path);
    bool saveInFile();
    void getData(); //get data of this one: need to save all mobs in one file
private:
    QString m_name;
    QString m_path;
};
}
#endif // EI_MOB_H
