#ifndef SCENE_H
#define SCENE_H

#include "ei_types.h"
#include "ei_object.h"
#include "ei_map.h"
#include "ei_mob.h"

class CScene{
    CScene();
public:

    //objects: model, sound, particle, trap (light?!)
    bool addObject(ei::CObjectInterface& obj);
    bool removeObject(ei::CObjectInterface& obj);
    bool hideObject(ei::CObjectInterface& obj, int id);
    bool copyObject(ei::CObjectInterface& obj);
    bool pasteObject(ei::CObjectInterface& obj);
    bool cutObject(ei::CObjectInterface& obj);
    QVector <ei::CObject> objects(); //for objects tree view;

    //map
    bool createMap();
    bool clearMap();

    //mob
    bool loadMob(QString path);
    bool saveMob(ei::CMob& mob);
    bool saveMobInOne(QString path);

    //common
    void saveAll();
    bool isChanged();
    void turn();
    void state();   //editing map/object/other


private:
    bool m_isChanged;
    QVector <ei::CObject> m_objects;
    QVector <ei::CMob> m_mobs;
    ei::CMap* m_map = nullptr;
}

#endif // SCENE_H
