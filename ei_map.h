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
    QString m_name;
    int m_Vertices; //change type
    int m_Tiles;   //change type
    QVector </*struct sector*/float > m_Sectors;

};

}

#endif // EI_MAP_H
