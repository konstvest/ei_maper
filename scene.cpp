#include "scene.h"

bool CScene::addObject(ei::CObjectInterface &obj){
    if (obj.add()){
        this->turn();
        return true;
    }
    return false;
}

bool CScene::removeObject(ei::CObjectInterface &obj){
    if (obj.remove()){
        this->turn();
        return true;
    }
    return false;
}

bool CScene::hideObject(ei::CObjectInterface &obj, int id){
    if (obj.hide())
        return true;
    return false;
}

bool CScene::copyObject(ei::CObjectInterface &obj){
    if (obj.copy())
        return true;
    return false;
}

bool CScene::pasteObject(ei::CObjectInterface &obj){
    if (obj.paste()){
        this->turn();
        return true;
    }
    return false;
}

bool CScene::cutObject(ei::CObjectInterface &obj){
    if (obj.cut()){
        this->turn();
        return true;
    }
    return false;
}
void CScene::saveAll(){
    for (auto mob=this->m_mobs.begin(); mob!=this->m_mobs.end(); ++mob)
    {
        mob->saveInFile();
    }

    m_isChanged = false;
}
bool CScene::isChanged(){
    return m_isChanged;
}

void CScene::turn(){
    m_isChanged = true;
}

bool CScene::createMap(){
    this->m_map->saveInFile("c:\\map.mpr");
    this->turn();
    return true;
}

bool CScene::clearMap(){
    this->turn();
    return true;
}
bool CScene::loadMob(QString path){
    this->turn();
    return true;
}
bool CScene::saveMob(ei::CMob& mob){

}
bool CScene::saveMobInOne(QString path);
