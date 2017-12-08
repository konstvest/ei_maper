#include "scene.h"

CScene::CScene():
    //m_map (0),
    m_name("unnamed")
{

}

CScene::~CScene(){
    //delete m_map;
    qDebug() << "Scene destructor\n";
}

bool CScene::addObject(ei::CObjectInterface* obj){
    if (obj->add()){
        this->turn();
        return true;
    }
    return false;
}

bool CScene::removeObject(ei::CObjectInterface* obj){
    if (obj->remove()){
        this->turn();
        return true;
    }
    return false;
}

bool CScene::hideObject(ei::CObjectInterface* obj, int id){
    obj->hide();
    return true;
}

bool CScene::copyObject(ei::CObjectInterface* obj){
    if (obj->copy())
        return true;
    return false;
}

bool CScene::pasteObject(ei::CObjectInterface* obj){
    if (obj->paste()){
        this->turn();
        return true;
    }
    return false;
}

bool CScene::cutObject(ei::CObjectInterface* obj){
    if (obj->cut()){
        this->turn();
        return true;
    }
    return false;
}

void CScene::draw(QOpenGLShaderProgram* shaders){
    foreach (ei::CFigure* fig, m_figures) {
        fig->draw(shaders);
    }
    //todo iterate by objects (NOT FIGURES) and draw()

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

//bool CScene::createMap(){
//    this->m_map->saveInFile("c:\\map.mpr");
//    this->turn();
//    return true;
//}

bool CScene::clearMap(){
    this->turn();
    return true;
}
bool CScene::loadMob(QString path){
    this->turn();
    return true;
}
bool CScene::saveMob(ei::CMob& mob){
    return true;
}
bool CScene::saveMobInOne(QString path){
    return true;
}

bool CScene::loadFigures(){
    ResFile res(m_figurePaths.first()); //change to loop for all figures
    QMap<QString, QByteArray> files = res.bufferOfFiles();
    ei::CFigure* fig = new ei::CFigure;
    QDataStream stream(files["tipy00.testfigloading"]);
    stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    fig->readData(stream);
    m_figures.push_back(fig);
    //m_objects.push_back(fig);

    return true;
}
