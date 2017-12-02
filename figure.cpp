#include "ei_object.h"

ei::CFigure::CFigure()
{

}

ei::CFigure::~CFigure()
{

}

struct SHeader{
    int vertBlocks = 0;
    int normalBlocks = 0;
    int UVCount = 0;
    int indexCount = 0;
    int vertexComponentCount = 0;
    int morphingComponentCount = 0;
    int unknown = 0;
    int group = 0;
    int textureNumber = 0;

    //todo read(QDataStream*& stream);
};

bool readSignature(FILE*& file){
    unsigned int sign;

    fread(&sign, sizeof(sign), 1, file);
    return sign == 0x38474946;
}

void readHeader(FILE*& file, SHeader& hd){
    fread(&hd, sizeof(hd), 1, file);
}

// read xyz(3) for morph components(8)
void read24(FILE*& file, QVector<f3>& points){
    f3 temp;

    for (int i(0); i<8; i++){
        fread(&temp, sizeof(temp), 1, file);
        points.push_back(temp);
    }
}

// read morph components(8)
void read8(FILE*& file, QVector<float>& points){
    float buf;

    for (int i(0); i<8;i++){
        fread(&buf, sizeof (buf), 1, file);
        points.push_back(buf);
    }
}

//     vertex#1           vertex#2
// m |{x00,y00,z00}| |{x10,y10,z10}| .......
// o |.............| |.............| .......
// r |.............| |.............| .......
// p |.............| |.............| .......
// h |{x07,y07,z07}| |{x17,y17,z17}| .......
void readVertices(FILE*& file, QVector <QVector <f3>>& morphVerts, const int blockCount){
    f3 pt;
    QVector<f3> vertices;
    float buf;

    vertices.fill(pt, blockCount*4);
    morphVerts.fill(vertices, 8);
    //mb change header[0] to header[morph component count]
    for (int block(0); block<blockCount; ++block){
        for (int xyz(0); xyz<3; ++xyz){
            for (int morph(0); morph<8; ++morph){
                for (int point(0); point<4; ++point){
                    fread(&buf, sizeof (buf), 1, file);
                    morphVerts[morph][block*4+point][xyz]=buf;
                }
            }
        }
    }
}

//     nrml#1          nrml#2
// |{x0,y0,z0,w0}| |{x1,y1,z1,w1}| ....
void readNormals(FILE*& file, QVector<f4>& nrmls, const int blockCount){
    f4 pt(1.0); //xyzw
    float buf;

    nrmls.fill(pt, blockCount*4);
    for (int block(0); block<blockCount; ++block){   //mb change header[0] to header[morph component count]
        for (int xyzw(0); xyzw<4; ++xyzw){
            for (int point(0); point<4; ++point){
                fread(&buf, sizeof (buf), 1, file);
                nrmls[block*4+point][xyzw]=buf;
            }
        }
    }
}

//     UV#1     UV#2
// |{x0,y0}| |{x1,y1}| ....
void readTextureCoords(FILE*& file, QVector<f2>& tCoords, const int uvCount){
    f2 pt;    //xy
    float buf;

    tCoords.fill(pt, uvCount);
    for (int point(0); point<uvCount; point++)
        for (int xy(0); xy<2; ++xy){
            fread(&buf, sizeof (buf), 1, file);
            tCoords[point][xy]=buf;
        }
}

void readIndices(FILE*& file, QVector<short>& indices, const int indCount){
    short buf;

    for (int i(0); i<indCount; i++){
        fread(&buf, sizeof(buf), 1, file);
        indices.push_back(buf);
    }
}

void readVertexComponents(FILE*& file, QVector<f3>& vComp, const int vcCount){
    short buf;
    f3 vertexComponent; // x==normal y==vertex z==texture

    for (int i(0); i<vcCount; ++i){
        for (int c(0); c<3; ++c){
            fread(&buf, sizeof(buf), 1, file);
            vertexComponent[c] = buf;
        }
        vComp.push_back(vertexComponent);
    }
}

//convert indices from ei_fig format to dif arrays of vertices, normals and texture coordinates
void convertIndices(QVector<short>& inds, QVector<f3>& comps, QVector<int>& normInds, QVector<int>& vertInds, QVector<int>& uvInds){
    for (int i(0); i<inds.size(); ++i){
        normInds.push_back(comps[inds[i]].x);
        vertInds.push_back(comps[inds[i]].y);
        uvInds.push_back(comps[inds[i]].z);
    }
}

int recalcUV(const int typeUV){
    switch (typeUV){
    case 2: return 1;
    case 8: return 2;
    default: return 0;
    }
}

void convertUVCoords(QVector<f2>& coordsUV, int convertCount){
    for (int i(0); i<convertCount; ++i){
        for (auto& pt:coordsUV){
            pt[0]/=2;
            pt[1]=0.5+pt[1]/2;
        }
    }
}

//load morphing_vertices, indices, normals, texture coordinates
bool ei::CFigure::loadFromFile(const wchar_t* path){
    SHeader header;
    QVector<short> fIndices;
    QVector<f3> vComponents;
    FILE* figFile = nullptr;

    figFile = _wfopen(path, L"rb");
    if (!figFile){
        qDebug() << "Can't load file " << path;
        return false;
    }
    //check signature
    if (!readSignature(figFile)){   // 0x38474946 == FIG8
        qDebug() << "incorrect signature";
        fclose(figFile);
        return false;
        }

    //read header
    readHeader(figFile, header);
    //read center
    read24(figFile, m_morphCenter);
    //read min
    read24(figFile, m_morphMin);
    //read max
    read24(figFile, m_morphMax);
    //read radius
    read8(figFile, m_morphRadius);
    //read vertices
    readVertices(figFile, m_morphVertices, header.vertBlocks);
    m_vertices.fill(f3(), m_morphVertices[0].size());
    //read normals
    readNormals(figFile, m_normals, header.normalBlocks);
    //read texture coordinates
    readTextureCoords(figFile, m_uvCoords, header.UVCount);
    //read indices
    readIndices(figFile, fIndices, header.indexCount);
    //read vertex components
    readVertexComponents(figFile, vComponents, header.vertexComponentCount);
    //create indicies array of vertices, uv coords and normals
    convertIndices(fIndices, vComponents, m_normIndices, m_vertIndices, m_uvIndices);
    //convert x,y uvCoords from type of object
    convertUVCoords(m_uvCoords, recalcUV(header.textureNumber));

    calculateConstitution(f3(1.0, 1.0, 1.0));

    fclose(figFile);
    return true;
}

void ei::CFigure::calculateConstitution(f3 constitute){ //x == str, y == dex, z == scale
    f3 res0, res1, res2;
    for (int i(0); i<m_morphVertices[0].size(); ++i){
        res0 = m_morphVertices[0][i] + (m_morphVertices[1][i] - m_morphVertices[0][i]) * constitute.x;
        res1 = m_morphVertices[2][i] + (m_morphVertices[3][i] - m_morphVertices[2][i]) * constitute.x;
        res2 = res0 + (res1 - res0) * constitute.y;
        res0 = m_morphVertices[4][i] + (m_morphVertices[5][i] - m_morphVertices[4][i]) * constitute.x;
        res1 = m_morphVertices[6][i] + (m_morphVertices[7][i] - m_morphVertices[6][i]) * constitute.x;
        res0 = res0 + (res1 - res0) * constitute.y;
        //need check it
        m_vertices[i] = res2 + (res0 - res2) * constitute.z;
    }
}

QVector<f3>& ei::CFigure::vertices(){
    return m_vertices;
}
