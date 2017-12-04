#ifndef SCENE_H
#define SCENE_H

#include "ei_types.h"
#include "ei_object.h"
#include "ei_map.h"
#include "ei_mob.h"
#include "figure.h"
#include "resfile.h"

class CScene{
public:
    CScene();
    ~CScene();
    //objects: model, sound, particle, trap (light?!)
    bool addObject(ei::CObjectInterface* obj);
    bool removeObject(ei::CObjectInterface* obj);
    bool hideObject(ei::CObjectInterface* obj, int id);
    bool copyObject(ei::CObjectInterface* obj);
    bool pasteObject(ei::CObjectInterface* obj);
    bool cutObject(ei::CObjectInterface* obj);
    QVector <ei::CObject> objects(); //for objects tree view;

    //map
    bool createMap();
    bool clearMap();

    //mob
    bool loadMob(QString path);
    bool saveMob(ei::CMob& mob);
    bool saveMobInOne(QString path);

    //res files
    bool loadResFile(QString path);

    //common
    void draw();
    void saveAll();
    bool isChanged();
    void turn();
    void state();   //editing map/object/other
    void setName(QString name) {m_name = name;}
    QString& name() {return m_name;}

private:
    bool m_isChanged;
    QVector <ei::CObject> m_objects;
    QVector <ei::CMob> m_mobs;
    ei::CMap* m_map;
    QString m_name;
};

#endif // SCENE_H
