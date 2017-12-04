#include "ei_object.h"

ei::CObject::CObject(){

}

ei::CObject::~CObject(){

}

bool ei::CObject::add(){
    return true;
}

bool ei::CObject::remove(){
    return true;
}

bool ei::CObject::move(f3& coord){

    return true;
}

bool ei::CObject::rotate(f4& quat){

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

void ei::CObject::draw(){

}

f3& ei::CObject::constitution(){
    return m_complex;
}

void ei::CObject::setConstitution(f3 constitution){

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
