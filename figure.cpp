#include "ei_object.h"

ei::CFigure::CFigure()
{

}

ei::CFigure::~CFigure()
{

}

// 0 - vertex block count
// 1 - normal block count
// 2 - texcoord count
// 3 - index count
// 4 - vertex component count
// 5 - morphing component count
// 6 - unknown
// 7 - group
// 8 - texture number
void readHeader(std::ifstream& file, QVector<int>& header){
    unsigned int buf;
    for (int i(0); i<9; i++){
        file.read((char*)&buf, sizeof (unsigned int));
        header.push_back(buf);
    }
}

// read xyz(3) for morph components(8)
void read24(std::ifstream& file, QVector<vec3>& points){
    vec3 temp;
    float buf;
    for (int i(0); i<8; i++){
        file.read((char*)&buf, sizeof (buf));
        temp.x = buf;
        file.read((char*)&buf, sizeof (buf));
        temp.y = buf;
        file.read((char*)&buf, sizeof (buf));
        temp.z = buf;
        points.push_back(temp);
    }
}

// read morph components(8)
void read8(std::ifstream& file, QVector<float>& points){
    float buf;
    for (int i(0); i<8;i++){
        file.read((char*)&buf, sizeof (buf));
        points.push_back(buf);
    }
}

//     vertex#1           vertex#2
// m |{x00,y00,z00}| |{x10,y10,z10}| .......
// o |.............| |.............| .......
// r |.............| |.............| .......
// p |.............| |.............| .......
// h |{x07,y07,z07}| |{x17,y17,z17}| .......
void readVertices(std::ifstream& file, QVector <QVector <QVector<float>>>& morphVerts, int blockCount){
    QVector<float> pt;  //xyz
    QVector<QVector<float>> vertices;   //array of pt
    float buf;
    //fill zeroes
    for (int coord(0); coord<3; ++coord)
        pt.append(0.0);
    for (int block(0); block<blockCount*4; ++block)
        vertices.append(pt);
    for (int morph(0); morph<8; ++morph)
        morphVerts.append(vertices);

    //mb change header[0] to header[morph component count]
    for (int block(0); block<blockCount; ++block){
        for (int xyz(0); xyz<3; ++xyz){
            for (int morph(0); morph<8; ++morph){
                for (int point(0); point<4; ++point){
                    file.read((char*)&buf, sizeof (buf));
                    morphVerts[morph][block*4+point][xyz]=buf;
                }
            }
        }
    }
}

//     nrml#1          nrml#2
// |{x0,y0,z0,w0}| |{x1,y1,z1,w1}| ....
void readNormals(std::ifstream& file, QVector<QVector<float>>& nrmls, int blockCount){
    QVector<float> pt; //xyzw
    float buf;
    //fill 1.0 for all normals
    for (int curN(0); curN<4; ++curN)
        pt.push_back(1.0);
    for (int block(0); block<blockCount*4; ++block)
        nrmls.push_back(pt);

    for (int block(0); block<blockCount; ++block){   //mb change header[0] to header[morph component count]
        for (int xyzw(0); xyzw<4; ++xyzw){
            for (int point(0); point<4; ++point){
                file.read((char*)&buf, sizeof (buf));
                nrmls[block*4+point][xyzw]=buf;
            }
        }
    }
}

//     UV#1     UV#2
// |{x0,y0}| |{x1,y1}| ....
void readTextureCoords(std::ifstream& file, QVector<QVector<float>>& tCoords, int uvCount){
    QVector<float> pt;    //xy
    float buf;
    //fill 0.0
    for (int t(0); t<2; ++t)
        pt.push_back(0.0);
    for (int pts(0); pts<uvCount; ++pts)
        tCoords.push_back(pt);

    for (int point(0); point<uvCount; point++)
        for (int xy(0); xy<2; ++xy){
            file.read((char*)&buf, sizeof (buf));
            tCoords[point][xy]=buf;
        }
}

void readIndices(std::ifstream& file, QVector<short>& indices, int indCount){
    short buf;
    for (int i(0); i<indCount; i++){
        file.read((char*)&buf, sizeof(buf));
        indices.push_back(buf);
    }
}

void readVertexComponents(std::ifstream& file, QVector<indices_link>& vComp, int vcCount){
    short buf;
    indices_link temp;
    for (int i(0); i<vcCount; i++){
        file.read((char*)&buf, sizeof(buf));
        temp.normal_ind = buf;
        file.read((char*)&buf, sizeof(buf));
        temp.vertex_ind = buf;
        file.read((char*)&buf, sizeof(buf));
        temp.texture_ind = buf;
        vComp.push_back(temp);
    }
}

//convert indices from ei_fig format to dif arrays for vertices, normals and texture coordinates
void convertIndices(QVector<short>& inds, QVector<indices_link>& comps, QVector<int>& vertInds, QVector<int>& normInds, QVector<int>& uvInds){
    for (int i(0); i<inds.size(); ++i){
        vertInds.push_back(comps[inds[i]].vertex_ind);
        normInds.push_back(comps[inds[i]].normal_ind);
        uvInds.push_back(comps[inds[i]].texture_ind);
    }
}

//load morphing_vertices, indices, normals, texture coordinates
bool ei::CFigure::loadFromFile(QString& pathFile){

    QVector<vec3> fCenter;
    QVector<vec3> fMin;
    QVector<vec3> fMax;
    QVector<float> fRadius;
    QVector<short> fIndices;
    QVector<indices_link> vComponents;


    QVector<QVector<QVector<float>>> preVertices;
    QVector<QVector<float>> preNormals;
    QVector<QVector<float>> preUVCoords;

    std::ifstream figFile;
    figFile.open(pathFile.toLatin1(), std::ios::binary);
    if (!figFile){
        qDebug() << "Can't load file " << pathFile;
        return false;
    }
    //check signature
    char signature[4];
    for (int i(0); i<4; i++)
        figFile>>signature[i];
    if (!strcmp(signature, "FIG8")){
        qDebug() << "incorrect signature";
        figFile.close();
        return false;
    }

    //read header
    readHeader(figFile, m_header);
    //read center
    read24(figFile, fCenter);
    //read min
    read24(figFile, fMin);
    //read max
    read24(figFile, fMax);
    //read radius
    read8(figFile, fRadius);
    //read vertices
    readVertices(figFile, preVertices, m_header[0]);
    //read normals
    readNormals(figFile, preNormals, m_header[1]);
    //read texture coordinates
    readTextureCoords(figFile, preUVCoords, m_header[2]);
    //read indices
    readIndices(figFile, fIndices, m_header[3]);
    //read vertex components
    readVertexComponents(figFile, vComponents, m_header[4]);
    //create indicies array of vertices, uv coords and normals
    convertIndices(fIndices, vComponents, m_vertIndices, m_normIndices, m_uvIndices);

    calculateConstitution(1.0, 1.0, 1.0);

    figFile.close();
    return true;
}

void ei::CFigure::calculateConstitution(float str, float dex, float scale){
    //TODO: fill m_vertices
}
