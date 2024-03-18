#ifndef FIGURE_H
#define FIGURE_H

#include <QDataStream>
#include <QVector>
#include <QVector4D>
#include "types.h"
#include "part.h"

namespace ei
{

struct SHeader{
    int vertBlocks = 0;
    int normalBlocks = 0;
    int uvCount = 0;
    int indexCount = 0;
    int vertexComponentCount = 0;
    int morphingComponentCount = 0;
    int unknown = 0;
    int group = 0;
    int textureNumber = 0;

    void read(QDataStream& stream);
};

///
/// \brief The CFigure class provides implementation of the internal 3D format of the game objects
///
class CFigure
{
public:
    CFigure();
    ~CFigure();
    //void getVertexData(QVector<SVertexData>& aVrtData, QVector3D& complection);
    void getVertexData(QVector<CPart*>& model, QVector3D& complection, QList<QString>& aBodyParts);
    void getMinimumBboxZ(float& value, QVector3D& complection);
    void uvCoords();
    void boundBox();
    void setComplex(float str, float dex, float tall);
    //TODO return methods of vertices, uv, normals, vert.indices, uv.indices
    bool readData(QDataStream& stream);
    void readAssemblyOffset(QDataStream& stream);
    void applyAssemblyOffset(QVector<QVector3D>* offset = nullptr);

    void addChild(ei::CFigure* fig) {m_aChild.append(fig); }
    QList<ei::CFigure*>& children() {return m_aChild; }
    void setName (const QString& name) {m_name = name; }
    QString& name() {return m_name;}
    void setOffset(QVector<QVector3D>& offset) {m_offset = offset; }
    QVector<QVector3D>& offset() {return m_offset; }
    void getPartNames(QStringList& arrBodyParts);

private:
    void generateTriangles(QVector<SVertexData>& aVrtData, QVector<QVector3D>& aMorphVertex);
    void calculateConstitution (QVector<SVertexData>& aVrtData, QVector3D& constitute);

private:
    //TODO: change vector to array
    //QVector<float> m_BoundBox;

    QVector<QVector4D> m_aNormal;
    QVector<QVector2D> m_aUvCoord;
    //QVector<QVector2D> m_aUvComp;   //converted uv coords to vertex data

    QVector<ushort> m_aVertInd;  //change to more container, not short
    //QVector<S3> m_vComponents;
    //QVector<int> m_normIndices;
    QVector<ushort> m_aUvInd;

    // vectors include morph components
    QVector<QVector<QVector3D>> m_aMorphVertex;    // [0-7][x,y,x] vertices
    //TODO: change vector to array or bbox class
    QVector<QVector3D> m_morphMin;   // 8x3    //todo: min max convert to bbox
    QVector<QVector3D> m_morphMax;   // 8x3
    QVector<QVector3D> m_morphCenter; //8x3
    //QVector<float> m_morphRadius;  //8

    QVector<QVector3D> m_offset; //8x3 assembly offset from bon file
    QVector<ushort> m_aInd;
    QVector<US3> m_aVertComp;
    QString m_name;
    QList<ei::CFigure*> m_aChild;

};

} // ei namespace

#endif // FIGURE_H
