#include "ei_object.h"

bool ei::CObject::add(){
    return true;
}

bool ei::CObject::remove(){
    return true;
}

bool ei::CObject::move(vec3){

    return true;
}

bool ei::CObject::rotate(vec4){

    return true;
}

bool ei::CObject::copy(){
    return true;
}

bool ei::CObject::paste(){
    return true;
}

bool ei::CObject::cut(){
    return true;
}

void ei::CObject::hide(){

}

void ei::CObject::show(){

}

bool ei::CObject::isHide(){

    return false;
}

bool ei::CObject::select(){
    return true;
}

int ei::CObject::id(){

    return 0;
}

double ei::CObject::scale(){

    return 0.0;
}

double ei::CObject::strenght(){

    return 0.0;
}


void ei::CObject::setStrenght(float d){

}

void ei::CObject::setDexterity(float d){

}

void ei::CObject::setScale(float d){

}

void ei::CObject::setName(const char* s){

}

bool ei::CObject::isQuest(){
    return true;
}

//path: Absolutely path to *.mod file
//return: True if saccessed read model
bool ei::CObject::loadFromFile(QString path){
    return true;
}
