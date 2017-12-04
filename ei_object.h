#ifndef EI_OBJECT_H
#define EI_OBJECT_H

#include "ei_types.h"
#include "figure.h"

namespace ei {

class CObjectInterface{
public:
    virtual bool add() = 0;
    virtual bool remove() = 0;
    virtual bool move(f3& coord) = 0;
    virtual bool rotate(f4& quat) = 0;
    virtual bool copy() = 0;
    virtual bool paste() = 0;
    virtual bool cut() = 0;
    virtual void hide() = 0;
    virtual void show() = 0;
    virtual bool isHide() = 0;
    virtual bool select() = 0;
    virtual void draw() = 0;
    //virtual bool clearComplex();
    //virtual bool clearPosition();
    //virtual bool clearRotation();
};

class CObject : public CObjectInterface{

public:
    CObject();
    ~CObject();
    // virtuals
    bool add();
    bool remove();
    bool move(f3& coord);
    bool rotate(f4& quat);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();
    void draw();
    // quiries
    int id();
    f3& constitution();
    double strenght();
    //sets
    void setConstitution(f3 constitution);
//    void setStrenght(float d);
//    void setDexterity(float d);
//    void setScale(float d);
    void setName(const char* s);
    bool isQuest();
    bool constructModel();   //construct model from loaded figures
    bool loadFromFile(QString path);    //for single import object

private:
    QString m_modelName;
    QVector <CFigure> m_figures; //mb use pointers
    QString m_globalVariable;
    QString m_modelPrototype;
    QString m_texturePrimary;
    QString m_textureSecondary;
    QString m_quest;
    QString m_name;    //name in game (need parse text.res)
    QString m_parentMob;
    int m_id;
    int m_player;
    int m_type;
    f3 m_complex;   // str, dex, tall
    f3 m_position;
    f4 m_rotation;    // quaternion x,y,z,w
};

class CUnit : public CObject{
public:
    CUnit();
    //virtuals
    bool add();
    bool remove();
    bool move(f3& coord);
    bool rotate(f4& quat);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();
    void draw();

    bool setStrenght(float d);
private:
    QString m_prototypeName;
    QString m_questItem; // ?!
    QVector <CFigure> m_figures; //mb use pointers

    //patrol point, agressive, ...
};

class CParticle : public CObjectInterface{

public:
    CParticle();
    //virtuals
    bool add();
    bool remove();
    bool move(f3& coord);
    bool rotate(f4& quat);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();
    void draw();
};

class CSound : public CObjectInterface{

public:
    CSound();
    //virtuals
    bool add();
    bool remove();
    bool move(f3& coord);
    bool rotate(f4& quat);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();
    void draw();
private:
    QVector <CFigure> m_figures; //mb use pointers
};

//class CLight : public CObjectInterface{

//public:
//    CLight();
//    //virtuals
//    bool add();
//    bool remove();
//    bool move(f3& coord);
//    bool rotate(f4& quat);
//    bool copy();
//    bool paste();
//    bool cut();
//    void hide();
//    void show();
//    bool isHide();
//    bool select();
//    void draw();
//public:
//    QVector <CFigure> m_figures; //mb use pointers
//};

class CTrap : public CObjectInterface{

public:
    CTrap();
    //virtuals
    bool add();
    bool remove();
    bool move(f3& coord);
    bool rotate(f4& quat);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();
    void draw();
private:
    float m_radius;
    float m_position;
    QVector <CFigure> m_figures; //mb use pointers
};

}

#endif // EI_OBJECT_H
