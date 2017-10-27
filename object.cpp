#include "ei_object.h"

bool ei::CObject::move(){

    return true;
}

bool ei::CObject::rotate(){

    return true;
}

bool ei::CObject::isHide(){

    return false;
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
