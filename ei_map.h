#ifndef EI_MAP_H
#define EI_MAP_H

#include "ei_types.h"

namespace ei {

class CMap{
public:
    CMap();
    bool loadFromFile();
    bool saveInFile(QString* path);
    bool vertices();
    bool name();

    void tiles();   //change return type to tiles
    bool addSector();
    bool removeSector();
    bool saveMinimap();
    bool loadHeightMap();
    bool saveHeightMap();

private:
    QString name;

};

}

#endif // EI_MAP_H
