#ifndef EI_OBJECT_H
#define EI_OBJECT_H

#include "ei_types.h"

namespace ei {

class CObjectInterface{
public:
    virtual bool add() = 0;
    virtual bool remove() = 0;
    virtual bool move(vec3) = 0;
    virtual bool rotate(vec4) = 0;
    virtual bool copy() = 0;
    virtual bool paste() = 0;
    virtual bool cut() = 0;
    virtual void hide() = 0;
    virtual void show() = 0;
    virtual bool isHide() = 0;
    virtual bool select() = 0;
    //virtual bool clearComplex();
    //virtual bool clearPosition();
    //virtual bool clearRotation();
};

class CFigure{
public:
    CFigure();
    ~CFigure();
    void vertices();
    void normals();
    void uvCoords();
    void boundBox();
    void setComplex(float str, float dex, float tall);
    //QVector <float> vertices;
    //QVector <vec4> normals;
    //QVector <vec2> t_coords;
    //QVector <unsigned int> indices;
    bool loadFromFile(QString pathFile);
    //void recalcConstitution (double str, double dex, double scale);
    //void recalcTextureCoordinates (QString type);
    //void convertToGLIndices ();
private:

    QVector <int> m_header;
    QVector <vec3> m_vertices;
    QVector<QVector<vec3>> m_morphingVertices;
    QVector<vec4> m_normals;
    //m_boundBoox;  // box of min and max coordinates
    /*QVector <vec3> min;
    QVector <vec3> max;
    QVector <vec3> center;
    QVector <float> radius;
    QVector <QVector <vec3>> preVertices;
    QVector <vec2> preT_coords;
    QVector <short> preIndices;
    QVector <indices_link> vert_comp;
    QVector <short> light_comp;*/
};

class CObject : public CObjectInterface{

public:
    // virtuals
    bool add();
    bool remove();
    bool move(vec3);
    bool rotate(vec4);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();
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
    vec3 m_complex;   // str, dex, tall
    vec3 m_position;    // x, y ,z
    vec4 m_rotation;    // quaternion x,y,z,w
};

class CUnit : public CObject{
public:
    CUnit();
    bool add();
    bool remove();
    bool move(vec3);
    bool rotate(vec4);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();

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
    bool move(vec3);
    bool rotate(vec4);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();
};

class CSound : public CObjectInterface{

public:
    CSound();
    //virtuals
    bool add();
    bool remove();
    bool move(vec3);
    bool rotate(vec4);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();
};

class CLight : public CObjectInterface{

public:
    CLight();
    //virtuals
    bool add();
    bool remove();
    bool move(vec3);
    bool rotate(vec4);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();
};

class CTrap : public CObjectInterface{

public:
    CTrap();
    //virtuals
    bool add();
    bool remove();
    bool move(vec3);
    bool rotate(vec4);
    bool copy();
    bool paste();
    bool cut();
    void hide();
    void show();
    bool isHide();
    bool select();
private:
    float m_radius;
    float m_position;
};

}

#endif // EI_OBJECT_H
