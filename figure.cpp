#include <QDebug>
#include <QVector4D>
#include "figure.h"

void ei::SHeader::read(QDataStream& stream)
{
    stream >> vertBlocks >> normalBlocks >> uvCount >> indexCount
            >> vertexComponentCount >> morphingComponentCount
            >> unknown >> group >> textureNumber;
}

ei::CFigure::CFigure()
{
    m_aChild.clear();
    m_offset.fill(QVector3D(0.0, 0.0, 0.0), 8);
}

ei::CFigure::~CFigure()
{
    // runtime error :(
    //    for (auto child: m_aChild)
    //        child->~CFigure();

    //m_aChild.clear();
    //delete this;
}

static bool readSignature(QDataStream& st)
{
    uint sign;
    st >> sign;
    return sign == 0x38474946;
}

// read xyz(3) for morph components(8)
static void read24(QDataStream& st, QVector<QVector3D>& points)
{
    QVector3D xyz;
    for(int i(0); i<8; ++i)
    {
        st >> xyz;
        points.append(xyz);
    }
}

// read morph components(8)
static void read8(QDataStream& st, QVector<float>& points)
{
    float buf;
    for(int i(0); i<8; ++i)
    {
        st >> buf;
        points.append(buf);
    }
}

/*     block#1       block#2
m |{x00,y00,z00}| |{x10,y10,z10}| .......
o |.............| |.............| .......
r |.............| |.............| .......
p |.............| |.............| .......
h |{x07,y07,z07}| |{x17,y17,z17}| .......*/
static void readVertices(QDataStream& st, QVector <QVector <QVector3D>>& morphVerts, const int blockCount)
{
    QVector3D pt;
    QVector<QVector3D> vertices;
    float buf;

    vertices.fill(pt, blockCount*4);
    morphVerts.fill(vertices, 8);
    //mb change header[0] to header[morph component count]
    for (int block(0); block<blockCount; ++block)
    {
        for (int xyz(0); xyz<3; ++xyz)
        {
            for (int morph(0); morph<8; ++morph)
            {
                for (int point(0); point<4; ++point)
                {
                    st >> buf;
                    morphVerts[morph][block*4+point][xyz]=buf;
                }
            }
        }
    }
}

//     nrml#1          nrml#2
// |{x0,y0,z0,w0}| |{x1,y1,z1,w1}| ....
static void readNormals(QDataStream& st, QVector<QVector4D>& aNormal, const int blockCount)
{
    QVector4D nrm(1.0f, 1.0f, 1.0f, 1.0f);
    float buf;
    aNormal.fill(nrm, blockCount*4);
    for(int block(0); block<blockCount; ++block)
    {   //mb change header[0] to header[morph component count]
        for(int xyzw(0); xyzw<4; ++xyzw)
        {
            for(int point(0); point<4; ++point)
            {
                st >> buf;
                aNormal[block*4+point][xyzw]=buf;
            }
        }
    }
}

//     UV#1     UV#2
// |{x0,y0}| |{x1,y1}| ....
static void readTextureCoords(QDataStream& st, QVector<QVector2D>& tCoords, const int uvCount)
{
    QVector2D uv;
    for(int point(0); point<uvCount; ++point)
    {
        st >> uv;
        tCoords.append(uv);
    }
}

static void readIndices(QDataStream& st, QVector<ushort>& indices, const int indCount)
{
    ushort buf;
    for (int i(0); i<indCount; i++)
    {
        st >> buf;
        indices.append(buf);
    }
}

static void readVertexComponents(QDataStream& st, QVector<US3>& vComp, const int vcCount)
{
    ushort buf;
    US3 vertexComponent; // x - normal, y - vertex, z - texture

    for (int i(0); i<vcCount; ++i)
    {
        for (int c(0); c<3; ++c)
        {
            st >> buf;
            vertexComponent[c] = buf;
        }
        vComp.append(vertexComponent);
    }
}

//convert x,y uvCoords from type of object
void convertUVCoords(QVector<F2>& coordsUV, int convertCount)
{
    for (int i(0); i<convertCount; ++i)
    { // wtf?
        for (auto& pt:coordsUV)
        {
            pt[0]/=2;
            pt[1]=0.5f + pt[1] / 2.0f;
        }
    }
}

//Creates trianlge from vertex component
// in. aIndOrigin - triangle indices
// in. aVertComp - vertex components
void ei::CFigure::generateTriangles(QVector<SVertexData>& aVrtData, QVector<QVector3D>& aMorphVertex)
{
    for(auto& ind: m_aInd)
        aVrtData.append(SVertexData(aMorphVertex[m_aVertComp[ind].x]
                          ,m_aNormal[m_aVertComp[ind].y]
                          ,m_aUvCoord[m_aVertComp[ind].z]));
}

//load morphing_vertices, indices, normals, texture coordinates
bool ei::CFigure::readData(QDataStream& stream)
{
    //check signature
    if(!readSignature(stream)){   // 0x38474946 == FIG8
        qDebug() << "Unknown fig signature";
        return false;
    }

    ei::SHeader header;
    header.read(stream);

    //read center
    //QVector<F3> bboxCentr;
    read24(stream, m_morphCenter);
    //read min
    read24(stream, m_morphMin);
    //read max
    read24(stream, m_morphMax);
    //read radius
    QVector<float> radius;
    read8(stream, radius);

    readVertices(stream, m_aMorphVertex, header.vertBlocks);
    readNormals(stream, m_aNormal, header.normalBlocks);
    readTextureCoords(stream, m_aUvCoord, header.uvCount);
    //QVector<ushort> aInd;
    readIndices(stream, m_aInd, header.indexCount);
    //QVector<US3> aVertComp;
    readVertexComponents(stream, m_aVertComp, header.vertexComponentCount);

    return true;
}

// gets assembly offset from bon-file
// in. stream - bon-file
void ei::CFigure::readAssemblyOffset(QDataStream& stream)
{
    m_offset.resize(8);
    QVector3D xyz;
    for (int i(0); i<8; ++i)
    {
        stream >> xyz;
        m_offset[i] = xyz;
    }
}

// Sets recursively assembly offset for all children
// in. offset - accumulate for children
void ei::CFigure::applyAssemblyOffset(QVector<QVector3D>* offset)
{
    if(offset)
    {
        Q_ASSERT((offset->size() == 8) && (m_offset.size() == 8));
        for(int i(0); i<offset->size(); ++i)
        {
            m_offset[i] += offset->at(i);
        }
    }
    for(auto& child: m_aChild)
        child->applyAssemblyOffset(&m_offset);
}

// Calculates point for current complection
// out. complexPoint - calculated point
// in. data - original point coordinate
// in. constitute - three-component complection
void calcComplection(QVector3D& complexPoint, QVector<QVector3D>& data, QVector3D& constitute)
{
    Q_ASSERT(data.size() == 8);
    QVector3D res0;
    QVector3D res1;
    QVector3D res2;
    res0 = data[0] + (data[1] - data[0]) * constitute.y();
    res1 = data[2] + (data[3] - data[2]) * constitute.y();
    res2 = res0 + (res1 - res0) * constitute.x();
    res0 = data[4] + (data[5] - data[4]) * constitute.y();
    res1 = data[6] + (data[7] - data[6]) * constitute.y();
    res0 = res0 + (res1 - res0) * constitute.x();
    complexPoint = res2 + (res0 - res2) * constitute.z();
}

//todo: request constitution from object
void ei::CFigure::calculateConstitution(QVector<SVertexData>& aVrtData, QVector3D& constitute)
{ //x == str, y == dex, z == scale
    QVector3D res0;
    QVector3D res1;
    QVector3D res2;

    QVector3D offset;
    Q_ASSERT(m_offset.size() == 8);
    calcComplection(offset, m_offset, constitute);
    QVector<QVector3D> aMorphVertex;
    if(m_aMorphVertex.empty())
    {
        //TODO: process unknown models (figures)
        return;
    }
    for (int i(0); i<m_aMorphVertex[0].size(); ++i){
        res0 = m_aMorphVertex[0][i] + (m_aMorphVertex[1][i] - m_aMorphVertex[0][i]) * constitute.y();
        res1 = m_aMorphVertex[2][i] + (m_aMorphVertex[3][i] - m_aMorphVertex[2][i]) * constitute.y();
        res2 = res0 + (res1 - res0) * constitute.x();
        res0 = m_aMorphVertex[4][i] + (m_aMorphVertex[5][i] - m_aMorphVertex[4][i]) * constitute.y();
        res1 = m_aMorphVertex[6][i] + (m_aMorphVertex[7][i] - m_aMorphVertex[6][i]) * constitute.y();
        res0 = res0 + (res1 - res0) * constitute.x();
        aMorphVertex.append(res2 + (res0 - res2) * constitute.z() + offset);
    }
    generateTriangles(aVrtData, aMorphVertex);
}

void ei::CFigure::getVertexData(QVector<CPart*>& model, QVector3D& complection, QList<QString>& aBodyParts)
{
    if (aBodyParts.isEmpty() || aBodyParts.contains(m_name)) // calc only visible body parts
    {
        CPart* part = new CPart();
        part->setName(m_name);
        calculateConstitution(part->vertData(), complection);
        part->update();
        model.append(part);
    }
    for(auto& child : m_aChild)
        child->getVertexData(model, complection, aBodyParts);
}

void ei::CFigure::getMinimumBboxZ(float& value, QVector3D& complection)
{
    QVector3D minPoint;
    QVector3D centr;
    QVector3D componentOffset;
    calcComplection(componentOffset, m_offset, complection);
    calcComplection(minPoint, m_morphMin, complection);
    calcComplection(centr, m_morphMin, complection);
    minPoint = -componentOffset + centr - minPoint;
    if (value < 0.0f && minPoint.z() < value)
        value = minPoint.z();
    else if (value > 0.0f && minPoint.z() > value)
        value = minPoint.z();
    else if (value == 0.0f)
        value = minPoint.z();
    for (auto& child : m_aChild)
        child->getMinimumBboxZ(value, complection);
}
