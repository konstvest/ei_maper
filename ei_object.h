#ifndef EI_OBJECT_H
#define EI_OBJECT_H

#include "ei_types.h"

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
    virtual void draw();
    //virtual bool clearComplex();
    //virtual bool clearPosition();
    //virtual bool clearRotation();
};

class CFigure{
public:
    CFigure();
    ~CFigure();
    QVector<f3>& vertices();
    void normals();
    void uvCoords();
    void boundBox();
    void setComplex(float str, float dex, float tall);
    //TODO return methods of vertices, uv, normals, vert.indices, uv.indices
    bool loadFromFile(const wchar_t* path);
    void calculateConstitution (f3 constitute);
    void draw();
private:
    //TODO: change vector to array
    QVector<float> m_BoundBox;

    QVector<f3> m_vertices;
    QVector<int> m_vertIndices;
    QVector<f4> m_normals;
    QVector<int> m_normIndices;
    QVector<f2> m_uvCoords;
    QVector<int> m_uvIndices;

    // vectors include morph components
    QVector<QVector<f3>> m_morphVertices;
    //TODO: change vector to array or bbox class
    QVector <f3> m_morphMin;   // 8x3
    QVector <f3> m_morphMax;   // 8x3
    QVector <f3> m_morphCenter; //8x3
    QVector <float> m_morphRadius;  //8
};

class CObject : public CObjectInterface{

public:
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
    double scale();
    double strenght();
    //sets
    void setStrenght(float d);
    void setDexterity(float d);
    void setScale(float d);
    void setName(const char* s);
    bool isQuest();
    bool constructModel();   //construct model from loaded figures
    bool loadFromFile(QString path);    //for single import object

private:
    QString m_modelName;
    QVector <CFigure> m_figures; //!!!!replace to model, not vector of figures
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
};

class CLight : public CObjectInterface{

public:
    CLight();
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
};

}

#endif // EI_OBJECT_H
